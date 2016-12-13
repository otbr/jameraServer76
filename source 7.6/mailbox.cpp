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

#include "mailbox.h"
#include "game.h"
#include "player.h"
#include "ioplayer.h"
#include "depot.h"
#include "town.h"
#include "configmanager.h"  

#include <sstream>

extern Game g_game;
extern ConfigManager g_config;

Mailbox::Mailbox(uint16_t _type) : Item(_type)
{
	//
}

Mailbox::~Mailbox()
{
	//
}

ReturnValue Mailbox::__queryAdd(int32_t index, const Thing* thing, uint32_t count,
	uint32_t flags) const
{
	if(const Item* item = thing->getItem()){
		if(canSend(item)){
			return RET_NOERROR;
		}
	}
	
	return RET_NOTPOSSIBLE;	
}

ReturnValue Mailbox::__queryMaxCount(int32_t index, const Thing* thing, uint32_t count, uint32_t& maxQueryCount,
	uint32_t flags) const
{
	maxQueryCount = std::max((uint32_t)1, count);
	return RET_NOERROR;
}

ReturnValue Mailbox::__queryRemove(const Thing* thing, uint32_t count, uint32_t flags) const
{
	return RET_NOTPOSSIBLE;
}

Cylinder* Mailbox::__queryDestination(int32_t& index, const Thing* thing, Item** destItem,
	uint32_t& flags)
{
	return this;
}

void Mailbox::__addThing(Thing* thing)
{
	return __addThing(0, thing);
}

void Mailbox::__addThing(int32_t index, Thing* thing)
{
	if(Item* item = thing->getItem()){
		if(canSend(item)){
			sendItem(item);
		}
	}
}

void Mailbox::__updateThing(Thing* thing, uint16_t itemId, uint32_t count)
{
	//
}

void Mailbox::__replaceThing(uint32_t index, Thing* thing)
{
	//
}

void Mailbox::__removeThing(Thing* thing, uint32_t count)
{
	//
}

void Mailbox::postAddNotification(Thing* thing, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
	getParent()->postAddNotification(thing, index, LINK_PARENT);
}

void Mailbox::postRemoveNotification(Thing* thing, int32_t index, bool isCompleteRemoval, cylinderlink_t link /*= LINK_OWNER*/)
{
	getParent()->postRemoveNotification(thing, index, isCompleteRemoval, LINK_PARENT);
}

bool Mailbox::sendItem(Item* item)
{
	std::string receiver = std::string("");
	uint32_t dp = 0;

	if(!getReceiver(item, receiver, dp)){
		return false;
	}

	if(receiver == "" || dp == 0){ /**No need to continue if its still empty**/
		return false;
	}
	
	uint32_t guid;
	if(!IOPlayer::instance()->getGuidByName(guid, receiver)){
		return false;
	}
	
	if(Player* player = g_game.getPlayerByName(receiver)){
		Depot* depot = player->getDepot(dp, true);

		if(depot){
			if(g_game.internalMoveItem(item->getParent(), depot, INDEX_WHEREEVER,
			        item, item->getItemCount(), NULL, FLAG_NOLIMIT) == RET_NOERROR){
				g_game.transformItem(item, item->getID() + 1); /**Change it to stamped!**/	
			}
			return true;
		}
	}
	else if(IOPlayer::instance()->playerExists(receiver)){
		Player* player = new Player(receiver, NULL);
		
		if(!IOPlayer::instance()->loadPlayer(player, receiver)){
			#ifdef __DEBUG_MAILBOX__
			std::cout << "Failure: [Mailbox::sendItem], can not load player: " << reciever << std::endl;
			#endif
			delete player;
			return false;
		}
		
		#ifdef __DEBUG_MAILBOX__
		std::string playerName = player->getName();
		if(g_game.getPlayerByName(playerName)){
			std::cout << "Failure: [Mailbox::sendItem], receiver is online: " << receiver << "," << playerName << std::endl;
			delete player;
			return false;
		}
		#endif

		Depot* depot = player->getDepot(dp, true);
		if(depot){
			if(g_game.internalMoveItem(item->getParent(), depot, INDEX_WHEREEVER,
			        item, item->getItemCount(), NULL, FLAG_NOLIMIT) == RET_NOERROR){
				g_game.transformItem(item, item->getID() + 1);
			}

			IOPlayer::instance()->savePlayer(player);
			
			delete player;			
			return true;
		}

		delete player;
	}
	
	return false;
}

bool Mailbox::getReceiver(Item* item, std::string& name, uint32_t& dp)
{
	if(!item){
		return false;
	}

	if(item->getID() == ITEM_PARCEL){ /**We need to get the text from the label incase its a parcel**/
		Container* parcel = item->getContainer();

		for(ItemList::const_iterator cit = parcel->getItems(); cit != parcel->getEnd(); cit++){
			if((*cit)->getID() == ITEM_LABEL){
				item = (*cit);
				
				if(item->getText() != ""){
					break;	
				}  
			} 
		}
	}
	else if(item->getID() != ITEM_LETTER){/**The item is somehow not a parcel or letter**/
		std::cout << "Mailbox::getReciver error, trying to get reciecer from unkown item! ID:: " << item->getID() << "." << std::endl;
		return false;
	}

	if(!item || item->getText() == "") /**No label/letter found or its empty.**/
		return false;

	std::string temp;
	std::istringstream iss(item->getText(), std::istringstream::in);

	std::string strTown = "";
	uint32_t curLine = 1;

	while(getline(iss, temp, '\n')){
		if(curLine == 1){
			name = temp;
		}
		else if(curLine == 2){
			strTown = temp;
		}
		else{
			break;
		}

		++curLine;
	}

	Town* town = Towns::getInstance().getTown(strTown);
	if(town && town->getTownID() != g_config.getNumber(ConfigManager::ROOK_TEMPLE_ID)) {
		dp = town->getTownID();
	}
	else{
		return false;
	}
	
	return true;
}

bool Mailbox::canSend(const Item* item) const
{
	if(item->getID() == ITEM_PARCEL || item->getID() == ITEM_LETTER){
		return true;
	}
	
	return false;
}
