//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////
#include "otpch.h"

#include "chat.h"
#include "configmanager.h"
#include "player.h"
#include "game.h"

extern ConfigManager g_config;
extern Game g_game;
extern Chat g_chat;

PrivateChatChannel::PrivateChatChannel(uint16_t channelId, std::string channelName):
ChatChannel(channelId, channelName)
{
	m_owner = 0;
}

bool PrivateChatChannel::isInvited(const Player* player)
{
	if(!player)
		return false;

	if(player->getGUID() == getOwner())
		return true;

	InvitedMap::iterator it = m_invites.find(player->getGUID());
	if(it != m_invites.end())
		return true;

	return false;
}

bool PrivateChatChannel::addInvited(Player* player)
{
	InvitedMap::iterator it = m_invites.find(player->getGUID());
	if(it != m_invites.end())
		return false;

	m_invites[player->getGUID()] = player;

	return true;
}

bool PrivateChatChannel::removeInvited(Player* player)
{
	InvitedMap::iterator it = m_invites.find(player->getGUID());
	if(it == m_invites.end())
		return false;

	m_invites.erase(it);
	return true;
}

void PrivateChatChannel::invitePlayer(Player* player, Player* invitePlayer)
{
	if(player != invitePlayer && addInvited(invitePlayer))
	{
		std::string msg = player->getName();
		msg += " invites you to ";
		msg += (player->getSex() == PLAYERSEX_FEMALE ? "her" : "his");
		msg += " private chat channel.";
		invitePlayer->sendTextMessage(MSG_INFO_DESCR, msg.c_str());

		msg = invitePlayer->getName();
		msg += " has been invited.";
		player->sendTextMessage(MSG_INFO_DESCR, msg.c_str());
	}
}

void PrivateChatChannel::excludePlayer(Player* player, Player* excludePlayer)
{
	if(player != excludePlayer && removeInvited(excludePlayer))
	{
		removeUser(excludePlayer);

		std::string msg = excludePlayer->getName();
		msg += " has been excluded.";
		player->sendTextMessage(MSG_INFO_DESCR, msg.c_str());

		excludePlayer->sendClosePrivate(getId());
	}
}

void PrivateChatChannel::closeChannel()
{
	UsersMap::iterator cit;
	for(cit = m_users.begin(); cit != m_users.end(); ++cit){
		cit->second->sendClosePrivate(getId());
	}
}

ChatChannel::ChatChannel(uint16_t channelId, std::string channelName)
{
	m_id = channelId;
	m_name = channelName;
}

bool ChatChannel::addUser(Player* player)
{
	UsersMap::iterator it = m_users.find(player->getID());
	if(it != m_users.end())
		return false;

	switch(m_id)
	{
		case CHANNEL_GUILD:
		{

			break;
		}

		case CHANNEL_RVR:
		case CHANNEL_TRADE:
		case CHANNEL_TRADE_ROOK:
		{
			ChatChannel* channel = g_chat.getChannel(player, m_id);
			if(!channel)
			{
				#ifdef __DEBUG_CHAT__
				std::cout << "ChatChannel::addUser - failed retrieving channel." << std::endl;
				#endif
				return false;
			}
			break;
		}

		default:
			break;
	}

	m_users[player->getID()] = player;
	return true;
}

bool ChatChannel::removeUser(Player* player, bool sendCloseChannel /*= false*/)
{
	UsersMap::iterator it = m_users.find(player->getID());
	if(it == m_users.end())
		return false;

	m_users.erase(it);

	if(sendCloseChannel){
		player->sendClosePrivate(getId());
	}

	
	return true;
}

bool ChatChannel::talk(Player* player, SpeakClasses type, const std::string& text, uint32_t time /*= 0*/)
{
	// Can't speak to a channel you're not connected to
	UsersMap::const_iterator iter = m_users.find(player->getID());
	if(iter == m_users.end())
		return false;

	// Add trade muted condition
	if(getId() == CHANNEL_TRADE || getId() == CHANNEL_TRADE_ROOK || getId() == CHANNEL_HELP){
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_TRADE_MUTED, 120000, 0);
		player->addCondition(condition);
	}

	UsersMap::iterator it;
	for(it = m_users.begin(); it != m_users.end(); ++it){
		it->second->sendToChannel(player, type, text, getId(), time);
	}

	return true;
}

Chat::Chat()
{
	// Create the default channels
	ChatChannel* newChannel;


	newChannel = new ChatChannel(CHANNEL_RVR, "Rule Violations");
	if(newChannel)
		m_normalChannels[CHANNEL_RVR] = newChannel;

	newChannel = new ChatChannel(CHANNEL_GAME_CHAT, "Game-Chat");
	if(newChannel)
		m_normalChannels[CHANNEL_GAME_CHAT] = newChannel;

	newChannel = new ChatChannel(CHANNEL_TRADE, "Trade");
	if(newChannel)
		m_normalChannels[CHANNEL_TRADE] = newChannel;

	newChannel = new ChatChannel(CHANNEL_RL_CHAT, "RL-Chat");
	if(newChannel)
		m_normalChannels[CHANNEL_RL_CHAT] = newChannel;

	newChannel = new ChatChannel(CHANNEL_TRADE_ROOK, "Trade-Rookgaard");
	if(newChannel)
		m_normalChannels[CHANNEL_TRADE_ROOK] = newChannel;

	newChannel = new ChatChannel(CHANNEL_HELP, "Help");
	if(newChannel)
		m_normalChannels[CHANNEL_HELP] = newChannel;

	newChannel = new PrivateChatChannel(CHANNEL_PRIVATE, "Private Chat Channel");
	if(newChannel)
		dummyPrivate = newChannel;
}

Chat::~Chat()
{
	for(NormalChannelMap::iterator it = m_normalChannels.begin(); it != m_normalChannels.end(); ++it)
		delete it->second;

	m_normalChannels.clear();
	for(GuildChannelMap::iterator it = m_guildChannels.begin(); it != m_guildChannels.end(); ++it)
		delete it->second;

	m_guildChannels.clear();
	
	for(PrivateChannelMap::iterator it = m_privateChannels.begin(); it != m_privateChannels.end(); ++it)
		delete it->second;

	m_privateChannels.clear();
	delete dummyPrivate;
}

ChatChannel* Chat::createChannel(Player* player, uint16_t channelId)
{
	if(getChannel(player, channelId))
		return NULL;

	switch(channelId)
	{
		case CHANNEL_GUILD:
		{
			ChatChannel* newChannel = NULL;
			if((newChannel = new ChatChannel(channelId, player->getGuildName())))
				m_guildChannels[player->getGuildId()] = newChannel;

			return newChannel;
		}

		case CHANNEL_PRIVATE:
		{
			//only 1 private channel for each premium player
			if(!player->isPremium() || getPrivateChannel(player))
				return NULL;

			//find a free private channel slot
			for(uint16_t i = 100; i < 10000; ++i)
			{
				if(m_privateChannels.find(i) == m_privateChannels.end())
				{
					PrivateChatChannel* newChannel = NULL;
					if((newChannel = new PrivateChatChannel(i, player->getName() + "'s Channel")))
					{
						newChannel->setOwner(player->getGUID());
						m_privateChannels[i] = newChannel;
					}

					return newChannel;
				}
			}
		}

		default:
			break;
	}

	return NULL;
}

bool Chat::deleteChannel(Player* player, uint16_t channelId)
{
	switch(channelId)
	{
		case CHANNEL_GUILD:
		{
			GuildChannelMap::iterator it = m_guildChannels.find(player->getGuildId());
			if(it == m_guildChannels.end())
				return false;

			delete it->second;
			m_guildChannels.erase(it);
			return true;
		}


		default:
		{
			PrivateChannelMap::iterator it = m_privateChannels.find(channelId);
			if(it == m_privateChannels.end())
				return false;

			it->second->closeChannel();

			delete it->second;
			m_privateChannels.erase(it);
			return true;
		}
	}

	return false;
}

ChatChannel* Chat::addUserToChannel(Player* player, uint16_t channelId)
{
	ChatChannel* channel = getChannel(player, channelId);
	if(channel && channel->addUser(player))
		return channel;

	return NULL;
}

bool Chat::removeUserFromChannel(Player* player, uint16_t channelId)
{
	ChatChannel* channel = getChannel(player, channelId);
	if(!channel || !channel->removeUser(player))
		return false;

	if(player && channel->getOwner() == player->getGUID())
		deleteChannel(player, channelId);

	return true;
}

void Chat::removeUserFromAllChannels(Player* player)
{
	ChannelList list = getChannelList(player);
	for(ChannelList::iterator it = list.begin(); it != list.end(); ++it)
	{
		ChatChannel* channel = (*it);
		if(!channel || !channel->removeUser(player))
			continue;

		if(player && channel->getOwner() == player->getGUID())
			deleteChannel(player, channel->getId());
	}
}

bool Chat::talkToChannel(Player* player, SpeakClasses type, const std::string& text, uint16_t channelId)
{
	ChatChannel* channel = getChannel(player, channelId);
	if(!channel || !player || text.length() < 1)
		return false;

	switch(channelId)
    {
		case CHANNEL_HELP:
		{
			// Help channel
			if(type == SPEAK_CHANNEL_Y && player->hasFlag(PlayerFlag_TalkOrangeHelpChannel)){
				type = SPEAK_CHANNEL_O;
			}
			break;
		}
		// Players can't speak in these channels while they're level 1
		// Also, there is a delay of 2 minutes for trade and trade rook
		case CHANNEL_TRADE:
		case CHANNEL_TRADE_ROOK:
		case CHANNEL_RL_CHAT:
		case CHANNEL_GAME_CHAT:
		{
			if(!player->hasFlag(PlayerFlag_CannotBeMuted))
            {
				if(player->getLevel() < 2)
                {
					player->sendCancel("You may not speak into channels as long as you are on level 1.");
					return true;
					break;
				}
				else if((channelId == CHANNEL_TRADE || channelId == CHANNEL_TRADE_ROOK) && player->hasCondition(CONDITION_TRADE_MUTED))
                {
					player->sendCancel("You may only place one offer in two minutes.");
					return true;
					break;
				}
			}
	    }

		default:
		{
			break;
		}
	}


	return channel->talk(player, type, text);
}

std::string Chat::getChannelName(Player* player, uint16_t channelId)
{
	if(ChatChannel* channel = getChannel(player, channelId))
		return channel->getName();

	return "";
}

ChannelList Chat::getChannelList(Player* player)
{
	ChannelList list;
	if(player->getGuildId() && player->getGuildName().length())
	{
		ChatChannel* channel = getChannel(player, CHANNEL_GUILD);
		if(channel)
			list.push_back(channel);
		else if((channel = createChannel(player, CHANNEL_GUILD)))
			list.push_back(channel);
	}

	for(NormalChannelMap::iterator it = m_normalChannels.begin(); it != m_normalChannels.end(); ++it)
	{
		if(it->first == CHANNEL_GAME_CHAT || it->first == CHANNEL_RL_CHAT || it->first == CHANNEL_HELP)
		{
			list.push_back(it->second);
			continue;
		}

		ChatChannel* channel = getChannel(player, it->first);
		if(channel)
			list.push_back(it->second);
	}

	bool hasPrivate = false;
	for(PrivateChannelMap::iterator pit = m_privateChannels.begin(); pit != m_privateChannels.end(); ++pit)
	{
		if(PrivateChatChannel* channel = pit->second)
		{
			if(channel->isInvited(player))
				list.push_back(channel);

			if(channel->getOwner() == player->getGUID())
				hasPrivate = true;
		}
	}

	if(!hasPrivate && player->isPremium())
		list.push_front(dummyPrivate);

	return list;
}

ChatChannel* Chat::getChannel(Player* player, uint16_t channelId)
{
	if(channelId == CHANNEL_GUILD)
	{
		GuildChannelMap::iterator git = m_guildChannels.find(player->getGuildId());
		if(git != m_guildChannels.end())
			return git->second;

		return NULL;
	}

	NormalChannelMap::iterator nit = m_normalChannels.find(channelId);
	if(nit != m_normalChannels.end())
	{
		switch(channelId)
		{

			case CHANNEL_RVR:
				if(!player->hasFlag(PlayerFlag_CanAnswerRuleViolations))
					return NULL;
				break;

			case CHANNEL_TRADE:
				if(player->getVocationId() == 0)
					return NULL;
				break;

			case CHANNEL_TRADE_ROOK:
				if(player->getVocationId() != 0)
					return NULL;
				break;
		}

		return nit->second;
	}

	PrivateChannelMap::iterator pit = m_privateChannels.find(channelId);
	if(pit != m_privateChannels.end() && pit->second->isInvited(player))
		return pit->second;

	return NULL;
}

ChatChannel* Chat::getChannelById(uint16_t channelId)
{
	NormalChannelMap::iterator it = m_normalChannels.find(channelId);
	if(it != m_normalChannels.end())
		return it->second;

	return NULL;
}

PrivateChatChannel* Chat::getPrivateChannel(Player* player)
{
	PrivateChatChannel* channel = NULL;
	for(PrivateChannelMap::iterator it = m_privateChannels.begin(); it != m_privateChannels.end(); ++it)
	{
		if((channel = it->second) && channel->getOwner() == player->getGUID())
			return channel;
	}

	return channel;
}
