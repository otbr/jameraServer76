//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Implementation of tibia v8.0 protocol
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


#ifndef __OTSERV_PROTOCOL76_H__
#define __OTSERV_PROTOCOL76_H__

#include <string>
#include "protocol.h"
#include "enums.h"
#include "creature.h"

enum connectResult_t{
	CONNECT_SUCCESS = 1,
	CONNECT_TOMANYPLAYERS = 2,
	CONNECT_MASTERPOSERROR = 3,
	CONNECT_INTERNALERROR = 4
};

class NetworkMessage;
typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;
class Player;
class Game;
class House;
class Container;
class Tile;
class Connection;

class Protocol76 : public Protocol
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t protocol76Count;
#endif

	Protocol76(Connection* connection);
	virtual ~Protocol76();

	bool login(const std::string& name);
	bool logout(bool forced);

	void setPlayer(Player* p);

private:
	std::list<uint32_t> knownCreatureList;

	bool connect(uint32_t playerId);
	void disconnectClient(uint8_t error, const char* message);
	void disconnect();

	virtual void releaseProtocol();
	virtual void deleteProtocolTask();

	void checkCreatureAsKnown(uint32_t id, bool &known, uint32_t &removedKnown);

	bool canSee(int x, int y, int z) const;
	bool canSee(const Creature*) const;
	bool canSee(const Position& pos) const;

	// we have all the parse methods
	virtual void parsePacket(NetworkMessage& msg);
	virtual void onRecvFirstMessage(NetworkMessage& msg);
	bool parseFirstPacket(NetworkMessage& msg);

	//Parse methods
	void parseLogout(NetworkMessage& msg);
	void parseCancelMove(NetworkMessage& msg);

	void parseRecievePing(NetworkMessage& msg);
	void parseAutoWalk(NetworkMessage& msg);
	void parseStopAutoWalk(NetworkMessage& msg);
	void parseMove(NetworkMessage& msg, Direction dir);

	void parseTurn(NetworkMessage& msg, Direction dir);

	void parseRequestOutfit(NetworkMessage& msg);
	void parseSetOutfit(NetworkMessage& msg);
	void parseSay(NetworkMessage& msg);
	void parseLookAt(NetworkMessage& msg);
	void parseFightModes(NetworkMessage& msg);
	void parseAttack(NetworkMessage& msg);
	void parseFollow(NetworkMessage& msg);

	void parseThrow(NetworkMessage& msg);
	void parseUseItemEx(NetworkMessage& msg);
	void parseBattleWindow(NetworkMessage& msg);
	void parseUseItem(NetworkMessage& msg);
	void parseCloseContainer(NetworkMessage& msg);
	void parseUpArrowContainer(NetworkMessage& msg);
	void parseUpdateTile(NetworkMessage& msg);
	void parseUpdateContainer(NetworkMessage& msg);
	void parseTextWindow(NetworkMessage& msg);
	void parseHouseWindow(NetworkMessage& msg);

	//party methods
	void parseInviteToParty(NetworkMessage& msg);
	void parseJoinParty(NetworkMessage& msg);
	void parseRevokePartyInvitation(NetworkMessage& msg);
	void parsePassPartyLeadership(NetworkMessage& msg);
	void parseLeaveParty(NetworkMessage& msg);

	//trade methods
	void parseRequestTrade(NetworkMessage& msg);
	void parseLookInTrade(NetworkMessage& msg);
	void parseAcceptTrade(NetworkMessage& msg);
	void parseCloseTrade();

	//VIP methods
	void parseAddVip(NetworkMessage& msg);
	void parseRemoveVip(NetworkMessage& msg);

	void parseRotateItem(NetworkMessage& msg);

	//Channel tabs
	void parseCreatePrivateChannel(NetworkMessage& msg);
	void parseChannelInvite(NetworkMessage& msg);
	void parseChannelExclude(NetworkMessage& msg);
	void parseGetChannels(NetworkMessage& msg);
	void parseOpenChannel(NetworkMessage& msg);
	void parseOpenPriv(NetworkMessage& msg);
	void parseCloseChannel(NetworkMessage& msg);
	void parseProcessRuleViolation(NetworkMessage& msg);
	void parseCloseRuleViolation(NetworkMessage& msg);
	void parseCancelRuleViolation(NetworkMessage& msg);

	void parseBugReport(NetworkMessage &msg);
	void parseViolationWindow(NetworkMessage &msg);

	void parseDebugAssert(NetworkMessage& msg);

	void parseDebug(NetworkMessage& msg);

	//Send functions
	void sendClosePrivate(uint16_t channelId);
	void sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName);

	void sendChannelsDialog();
	void sendChannel(uint16_t channelId, const std::string& channelName);
	void sendRuleViolationsChannel(uint16_t channelId);
	void sendOpenPrivateChannel(const std::string& receiver);
	void sendToChannel(const Creature* creature, SpeakClasses type, const std::string& text, uint16_t channelId, uint32_t time = 0);
	void sendRemoveReport(const std::string& name);
	void sendLockRuleViolation();
	void sendRuleViolationCancel(const std::string& name);
	void sendIcons(int icons);

	void sendDistanceShoot(const Position& from, const Position& to, unsigned char type);
	void sendMagicEffect(const Position& pos, unsigned char type);
	void sendAnimatedText(const Position& pos, unsigned char color, std::string text);
	void sendCreatureHealth(const Creature* creature);
	void sendSkills();
	void sendPing();
	void sendCreatureTurn(const Creature* creature, unsigned char stackpos);
	void sendCreatureSay(const Creature* creature, SpeakClasses type, const std::string& text);

	void sendCancel(const std::string& message);
	void sendCancelWalk();
	void sendChangeSpeed(const Creature* creature, uint32_t speed);
	void sendCancelTarget();
	void sendCreatureVisible(const Creature* creature, bool visible);
	void sendCreatureOutfit(const Creature* creature, const Outfit_t& outfit);
	void sendCreatureInvisible(const Creature* creature);
	void sendStats();
	void sendTextMessage(MessageClasses mclass, const std::string& message);

	void sendTradeItemRequest(const Player* player, const Item* item, bool ack);
	void sendCloseTrade();

	void sendTextWindow(uint32_t windowTextId, Item* item, uint16_t maxlen, bool canWrite);
	void sendTextWindow(uint32_t windowTextId, uint32_t itemId, const std::string& text);
	void sendHouseWindow(uint32_t windowTextId, House* house, uint32_t listId, const std::string& text);
	void sendOutfitWindow(const Player* player);

	void sendVIPLogIn(uint32_t guid);
	void sendVIPLogOut(uint32_t guid);
	void sendVIP(uint32_t guid, const std::string& name, bool isOnline);
	void sendReLoginWindow();

	void sendCreatureLight(const Creature* creature);
	void sendWorldLight(const LightInfo& lightInfo);

	void sendCreatureSkull(const Creature* creature);
	void sendCreatureShield(const Creature* creature);
	void sendCreatureSquare(const Creature* creature, SquareColor_t color);

	//tiles
	void sendAddTileItem(const Tile* tile, const Position& pos, const Item* item);
	void sendUpdateTileItem(const Tile* tile, const Position& pos, uint32_t stackpos, const Item* item);
	void sendRemoveTileItem(const Tile* tile, const Position& pos, uint32_t stackpos);
	void sendUpdateTile(const Tile* tile, const Position& pos);

	void sendAddCreature(const Creature* creature, bool isLogin);
	void sendRemoveCreature(const Creature* creature, const Position& pos, uint32_t stackpos, bool isLogout);
	void sendMoveCreature(const Creature* creature, const Tile* newTile, const Position& newPos,
		const Tile* oldTile, const Position& oldPos, uint32_t oldStackPos, bool teleport);

	//containers
	void sendAddContainerItem(uint8_t cid, const Item* item);
	void sendUpdateContainerItem(uint8_t cid, uint8_t slot, const Item* item);
	void sendRemoveContainerItem(uint8_t cid, uint8_t slot);

	void sendContainer(uint32_t cid, const Container* container, bool hasParent);
	void sendCloseContainer(uint32_t cid);

	//inventory
	void sendAddInventoryItem(slots_t slot, const Item* item);
	void sendUpdateInventoryItem(slots_t slot, const Item* item);
	void sendRemoveInventoryItem(slots_t slot);

	//Help functions

	// translate a tile to clientreadable format
	void GetTileDescription(const Tile* tile, NetworkMessage_ptr msg);

	// translate a floor to clientreadable format
	void GetFloorDescription(NetworkMessage_ptr msg, int x, int y, int z,
		int width, int height, int offset, int& skip);

	// translate a map area to clientreadable format
	void GetMapDescription(unsigned short x, unsigned short y, unsigned char z,
		unsigned short width, unsigned short height, NetworkMessage_ptr msg);

	void AddMapDescription(NetworkMessage_ptr msg, const Position& pos);
	void AddTextMessage(NetworkMessage_ptr msg,MessageClasses mclass, const std::string& message);
	void AddAnimatedText(NetworkMessage_ptr msg,const Position& pos, unsigned char color, const std::string& text);
	void AddMagicEffect(NetworkMessage_ptr msg,const Position& pos, unsigned char type);
	void AddDistanceShoot(NetworkMessage_ptr msg,const Position& from, const Position& to, uint8_t type);
	void AddCreature(NetworkMessage_ptr msg,const Creature* creature, bool known, uint32_t remove);
	void AddPlayerStats(NetworkMessage_ptr msg);
	void AddCreatureSpeak(NetworkMessage_ptr msg, const Creature* creature, SpeakClasses type, std::string text, uint16_t channelId, uint32_t time = 0);
	void AddCreatureHealth(NetworkMessage_ptr msg,const Creature* creature);
	void AddCreatureOutfit(NetworkMessage_ptr msg, const Creature* creature, const Outfit_t& outfit);
	void AddCreatureInvisible(NetworkMessage_ptr msg, const Creature* creature);
	void AddPlayerSkills(NetworkMessage_ptr msg);
	void AddWorldLight(NetworkMessage_ptr msg, const LightInfo& lightInfo);
	void AddCreatureLight(NetworkMessage_ptr msg, const Creature* creature);

	//tiles
	void AddTileItem(NetworkMessage_ptr msg, const Position& pos, const Item* item);
	void AddTileCreature(NetworkMessage_ptr msg, const Position& pos, const Creature* creature);
	void UpdateTileItem(NetworkMessage_ptr msg, const Position& pos, uint32_t stackpos, const Item* item);
	void RemoveTileItem(NetworkMessage_ptr msg, const Position& pos, uint32_t stackpos);

	void MoveUpCreature(NetworkMessage_ptr msg, const Creature* creature,
		const Position& newPos, const Position& oldPos, uint32_t oldStackPos);
	void MoveDownCreature(NetworkMessage_ptr msg, const Creature* creature,
		const Position& newPos, const Position& oldPos, uint32_t oldStackPos);

	//container
	void AddContainerItem(NetworkMessage_ptr msg, uint8_t cid, const Item* item);
	void UpdateContainerItem(NetworkMessage_ptr msg, uint8_t cid, uint8_t slot, const Item* item);
	void RemoveContainerItem(NetworkMessage_ptr msg, uint8_t cid, uint8_t slot);

	//inventory
	void AddInventoryItem(NetworkMessage_ptr msg, slots_t slot, const Item* item);
	void UpdateInventoryItem(NetworkMessage_ptr msg, slots_t slot, const Item* item);
	void RemoveInventoryItem(NetworkMessage_ptr msg, slots_t slot);

	friend class Player;

	template<class T1, class f1, class r>
	void addGameTask(r (Game::*f)(f1), T1 p1);

	template<class T1, class T2, class f1, class f2, class r>
	void addGameTask(r (Game::*f)(f1, f2), T1 p1, T2 p2);

	template<class T1, class T2, class T3, class f1, class f2, class f3,
	class r>
	void addGameTask(r (Game::*f)(f1, f2, f3), T1 p1, T2 p2, T3 p3);

	template<class T1, class T2, class T3, class T4, class f1, class f2,
	class f3, class f4, class r>
	void addGameTask(r (Game::*f)(f1, f2, f3, f4), T1 p1, T2 p2, T3 p3, T4 p4);

	template<class T1, class T2, class T3, class T4, class T5, class f1,
	class f2, class f3, class f4, class f5, class r>
	void addGameTask(r (Game::*f)(f1, f2, f3, f4, f5), T1 p1, T2 p2, T3 p3, T4 p4, T5 p5);

	template<class T1, class T2, class T3, class T4, class T5, class T6,
	class f1, class f2, class f3, class f4, class f5, class f6, class r>
	void addGameTask(r (Game::*f)(f1, f2, f3, f4, f5, f6), T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6);

	template<class T1, class T2, class T3, class T4, class T5, class T6,
	class T7, class f1, class f2, class f3, class f4, class f5, class f6,
	class f7, class r>
	void addGameTask(r (Game::*f)(f1, f2, f3, f4, f5, f6, f7), T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7);

	template<class T1, class T2, class T3, class T4, class T5, class T6,
	class T7, class T8, class f1, class f2, class f3, class f4, class f5,
	class f6, class f7, class f8, class r>
	void addGameTask(r (Game::*f)(f1, f2, f3, f4, f5, f6, f7, f8), T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8);

	Player* player;

	int64_t m_now;
	int64_t m_nextTask;
	int64_t m_nextPing;

	int64_t m_lastTaskCheck;
	int32_t m_messageCount;
	int32_t m_rejectCount;
	uint32_t eventConnect;

	bool m_debugAssertSent;
	bool m_acceptPackets;
};

#endif
