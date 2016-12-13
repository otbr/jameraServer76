//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// class representing the gamestate
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

#include "definitions.h"

#include <string>
#include <sstream>
#include <map>
#include <fstream>

#include <boost/config.hpp>
#include <boost/bind.hpp>

#include "otsystem.h"
#include "tasks.h"
#include "items.h"
#include "commands.h"
#include "creature.h"
#include "player.h"
#include "monster.h"
#include "game.h"
#include "tile.h"
#include "house.h"
#include "actions.h"
#include "combat.h"
#include "ioplayer.h"
#include "chat.h"
#include "talkaction.h"
#include "spells.h"
#include "configmanager.h"
#include "server.h"
#include "party.h"
#include "ban.h"
#include "raids.h"
#include "spawn.h"
#include "ioaccount.h"
#include "tools.h"

#if defined __EXCEPTION_TRACER__
#include "exception.h"
extern boost::recursive_mutex maploadlock;
#endif

extern ConfigManager g_config;
extern Server* g_server;
extern Actions* g_actions;
extern Commands commands;
extern BanManager g_bans;
extern Chat g_chat;
extern TalkActions* g_talkactions;
extern Spells* g_spells;

Game::Game()
{
	gameState = GAME_STATE_NORMAL;
	map = NULL;
	worldType = WORLD_TYPE_PVP;

	lastPlayersRecord = 0;
	last_bucket = 0;
	int daycycle = 3600;
	//(1440 minutes/day)/(3600 seconds/day)*10 seconds event interval
	light_hour_delta = 1440*10/daycycle;
	/*light_hour = 0;
	lightlevel = LIGHT_LEVEL_NIGHT;
	light_state = LIGHT_STATE_NIGHT;*/
	light_hour = SUNRISE + (SUNSET - SUNRISE)/2;
	lightlevel = LIGHT_LEVEL_DAY;
	light_state = LIGHT_STATE_DAY;

	Scheduler::getScheduler().addEvent(createSchedulerTask(EVENT_LIGHTINTERVAL,
		boost::bind(&Game::checkLight, this)));
	checkCreatureLastIndex = 0;

	Scheduler::getScheduler().addEvent(createSchedulerTask(EVENT_CREATURE_THINK_INTERVAL,
		boost::bind(&Game::checkCreatures, this)));

	Scheduler::getScheduler().addEvent(createSchedulerTask(EVENT_DECAYINTERVAL,
		boost::bind(&Game::checkDecay, this)));
}

Game::~Game()
{
	if(map){
		delete map;
	}
}

void Game::setWorldType(WorldType_t type)
{
	worldType = type;
}

GameState_t Game::getGameState()
{
	return gameState;
}

void Game::setGameState(GameState_t newState)
{
	if(gameState == GAME_STATE_SHUTDOWN){
		//Can't go back from this state.
		return;
	}

	if(gameState != newState){
		switch(newState){
			case GAME_STATE_INIT:
			{
				Spawns::getInstance()->startup();

				Raids::getInstance()->loadFromXml(g_config.getString(
					ConfigManager::DATA_DIRECTORY) + "/raids/raids.xml");
				Raids::getInstance()->startup();
				
				loadGameState();
				break;
			}

			case GAME_STATE_SHUTDOWN:
			{
				//kick all players that are still online
				AutoList<Player>::listiterator it = Player::listPlayer.list.begin();
				while(it != Player::listPlayer.list.end()){
					(*it).second->kickPlayer();
					it = Player::listPlayer.list.begin();
				}
				
				saveGameState(true, false, false);

				Dispatcher::getDispatcher().addTask(createTask(
					boost::bind(&Game::shutdown, this)));
				Scheduler::getScheduler().stop();
				Dispatcher::getDispatcher().stop();
				break;
			}

			case GAME_STATE_STARTUP:
			case GAME_STATE_CLOSED:
			case GAME_STATE_CLOSING:
			case GAME_STATE_NORMAL:
			default:
				break;
		}
		gameState = newState;
	}
}

/*void Game::saveGameState()
{
	ScriptEnviroment::saveGameState();
}*/

void Game::callSave()
{
    //broadcast message if set to
    if(g_config.getBool(ConfigManager::SAVE_WARNING)){
        std::stringstream saveBroadcast;
        saveBroadcast << "Server is saving. Please wait...";
        anonymousBroadcastMessage(MSG_STATUS_WARNING, saveBroadcast.str());
    }
    
    //call saveGameState
    Scheduler::getScheduler().addEvent(createSchedulerTask(500, 
        boost::bind(&Game::saveGameState, this, true, true, true)));
}

void Game::saveGameState(bool saveMap, bool savePlayers, bool countFrequence)
{    
    //set start time
    uint64_t start = OTSYS_TIME();
    
    //clear unactive bans
    //g_bans.clearUnactiveBans();
    
    //clear deaths
    //IOPlayer::instance()->clearDeaths();
    
	//save all players online
    if(savePlayers)
	{
		for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it)
		{
			(*it).second->loginPosition = (*it).second->getPosition();
			IOPlayer::instance()->savePlayer((*it).second);
		}
	}
	
	//pay houses
    Houses::getInstance().payHouses();
	
    //save map
    if(saveMap)
        getMap()->saveMap();
    
    //save global storage 
    ScriptEnviroment::saveGameState();
    
    //show message
    std::cout << "[Server Save] Server has been saved in " << OTSYS_TIME() - start << " ms." << std::endl;   
         
    //next save
    uint32_t saveFrequence =  g_config.getNumber(ConfigManager::SIMPLE_SAVE_FREQUENCE);
    if(countFrequence && saveFrequence > 0){
        Scheduler::getScheduler().addEvent(createSchedulerTask((saveFrequence * 60000) - 500, 
            boost::bind(&Game::callSave, this)));
    }
}

void Game::cleanGameState(uint32_t minutes, bool countFrequence)
{
    if(minutes > 0){
        std::stringstream messsageBeforeClean;
        messsageBeforeClean << "Server will be cleaned in " << minutes << " minute(s). Please pick up your items from the floor!";
        anonymousBroadcastMessage(MSG_STATUS_WARNING, messsageBeforeClean.str());
        Scheduler::getScheduler().addEvent(createSchedulerTask(60000, boost::bind(&Game::cleanGameState, this, minutes - 1, countFrequence)));
    }
    else{
        cleanMap();
        int32_t autoCleanFrequence = g_config.getNumber(ConfigManager::AUTO_CLEAN_FREQUENCE);
        if(countFrequence && autoCleanFrequence > 0){
            int32_t cleanWarnBefore = g_config.getNumber(ConfigManager::CLEAN_WARN_BEFORE);
            if(cleanWarnBefore > 0 && cleanWarnBefore < autoCleanFrequence)
                Scheduler::getScheduler().addEvent(createSchedulerTask((autoCleanFrequence * 60000) - (cleanWarnBefore * 60000), boost::bind(&Game::cleanGameState, this, cleanWarnBefore, true)));
            else
                Scheduler::getScheduler().addEvent(createSchedulerTask(autoCleanFrequence * 60000, boost::bind(&Game::cleanGameState, this, 0, true)));
        }
    }
}

uint32_t Game::cleanMap()
{
   uint32_t returnCleanedItems = getMap()->cleanMap();
   std::cout << ":: Cleaned " << returnCleanedItems << " items from the map" << std::endl;
   std::stringstream cleanMessage;
   cleanMessage << "Cleaned " << returnCleanedItems << " items from the map";
   anonymousBroadcastMessage(MSG_STATUS_WARNING, cleanMessage.str());
}

void Game::beforeServerSave(uint32_t minutes)
{
    if(minutes > 0){
        std::stringstream messsageBeforeSave;
        messsageBeforeSave << "Server will be saved and closed in " << minutes << " minute(s). Please logout.";
        anonymousBroadcastMessage(MSG_STATUS_WARNING, messsageBeforeSave.str());
        Scheduler::getScheduler().addEvent(createSchedulerTask(60000, boost::bind(&Game::beforeServerSave, this, minutes - 1)));
    }
    else{
        serverSaveNow();
    }
}

void Game::serverSaveNow()
{
    if(g_config.getString(ConfigManager::SERVER_SAVE_MODE) != "shutdown")
    {
        std::cout << "[Server Save] Saving sever..." << std::endl;
        
        setGameState(GAME_STATE_CLOSED);
        
        std::cout << "[Server Save] Game state set to CLOSED..." << std::endl;
        std::cout << "[Server Save] Kicking and saving players..." << std::endl;
		AutoList<Player>::listiterator it = Player::listPlayer.list.begin();
		while(it != Player::listPlayer.list.end()){
            (*it).second->kickPlayer();
			it = Player::listPlayer.list.begin();
		}
		
        std::cout << "[Server Save] Saving bans, houses and map..." << std::endl;
        g_bans.clearTemporaryBans();
        saveGameState(true, false, false);       
        
        if(g_config.getBool(ConfigManager::CLEAN_SERVER_ATSAVE))
        {
            std::cout << "[Server Save] Cleaning server..." << std::endl;
            getMap()->cleanMap();
        }
        
        if(g_config.getBool(ConfigManager::DATABASE_BACKUP_ATSAVE))
        {
            std::cout << "[Server Save] Backuping database..." << std::endl;
            system("backup.bat");
        }
        
        uint32_t warnBefore = g_config.getNumber(ConfigManager::SAVE_WARN_BEFORE);
        Scheduler::getScheduler().addEvent(createSchedulerTask(86400000 - (warnBefore * 60000), boost::bind(&Game::beforeServerSave, this, warnBefore)));
        std::cout << "[Server Save] Server saved successfully!" << std::endl;
        
        setGameState(GAME_STATE_NORMAL);
    }
    else{
        setGameState(GAME_STATE_SHUTDOWN); 
    }
} 

void Game::loadGameState()
{
	ScriptEnviroment::loadGameState();
}

int Game::loadMap(std::string filename, std::string filekind)
{
	if(!map){
		map = new Map;
	}

	maxPlayers = g_config.getNumber(ConfigManager::MAX_PLAYERS);
	inFightTicks = g_config.getNumber(ConfigManager::PZ_LOCKED);
	exhaustionTicks = g_config.getNumber(ConfigManager::EXHAUSTED);
	fightExhaustionTicks = g_config.getNumber(ConfigManager::FIGHTEXHAUSTED);
	healExhaustionTicks = g_config.getNumber(ConfigManager::HEALEXHAUSTED);
	Player::maxMessageBuffer = g_config.getNumber(ConfigManager::MAX_MESSAGEBUFFER);
	Monster::despawnRange = g_config.getNumber(ConfigManager::DEFAULT_DESPAWNRANGE);
	Monster::despawnRadius = g_config.getNumber(ConfigManager::DEFAULT_DESPAWNRADIUS);

	return map->loadMap(filename, filekind);
}

void Game::refreshMap(Map::TileMap::iterator* map_iter, int clean_max)
{
	Tile* tile;
	Item* item;


	Map::TileMap::iterator begin_here = map->refreshTileMap.begin();
	if(!map_iter)
		map_iter = &begin_here;
	Map::TileMap::iterator end_here = map->refreshTileMap.end();

	int cleaned = 0;
	for(; *map_iter != end_here && (clean_max == 0? true : (cleaned < clean_max)); ++*map_iter, ++cleaned){
		tile = (*map_iter)->first;

		//remove garbage
		int32_t downItemSize = tile->downItems.size();
		for(int32_t i = downItemSize - 1; i >= 0; --i){
			item = tile->downItems[i];
			if(item){
#ifndef __DEBUG__
				// So the compiler doesn't generates warnings
				internalRemoveItem(item);
#else
				ReturnValue ret = internalRemoveItem(item);
				if(ret != RET_NOERROR){
					std::cout << "Could not refresh item: " << item->getID() << "pos: " << tile->getPosition() << std::endl;
				}
#endif
			}
		}

		cleanup();

		//restore to original state
		ItemVector list = (*map_iter)->second.list;
		for(ItemVector::reverse_iterator it = list.rbegin(); it != list.rend(); ++it){
			Item* item = (*it)->clone();
			ReturnValue ret = internalAddItem(tile, item , INDEX_WHEREEVER, FLAG_NOLIMIT);
			if(ret == RET_NOERROR){
				if(item->getUniqueId() != 0){
					ScriptEnviroment::addUniqueThing(item);
				}
				startDecay(item);
			}
			else{
				std::cout << "Could not refresh item: " << item->getID() << "pos: " << tile->getPosition() << std::endl;
				delete item;
			}
		}
	}
}

void Game::proceduralRefresh(Map::TileMap::iterator* begin)
{
	if(!begin){
		begin = new Map::TileMap::iterator(map->refreshTileMap.begin());
	}

	// Refresh 250 tiles each cycle
	refreshMap(begin, 250);
	
	if(*begin == map->refreshTileMap.end()){
		delete begin;
		return;
	}

	// Refresh some items every 500 ms until all tiles has been checked
	// For 100k tiles, this would take 100000/2500 = 40s = half a minute
	Scheduler::getScheduler().addEvent(createSchedulerTask(100,
		boost::bind(&Game::proceduralRefresh, this, begin)));
}

/*****************************************************************************/

Cylinder* Game::internalGetCylinder(Player* player, const Position& pos)
{
	if(pos.x != 0xFFFF){
		return getTile(pos.x, pos.y, pos.z);
	}
	else{
		//container
		if(pos.y & 0x40){
			uint8_t from_cid = pos.y & 0x0F;
			return player->getContainer(from_cid);
		}
		//inventory
		else{
			return player;
		}
	}
}

Thing* Game::internalGetThing(Player* player, const Position& pos, int32_t index,
	uint32_t spriteId /*= 0*/, stackPosType_t type /*= STACKPOS_NORMAL*/)
{
	if(pos.x != 0xFFFF){
		Tile* tile = getTile(pos.x, pos.y, pos.z);

		if(tile){
			/*look at*/
			if(type == STACKPOS_LOOK){
#ifdef __PB_GMINVISIBLE__
				Creature* c;
				if(tile->getTopThing() && (c = tile->getTopThing()->getCreature())){
					if(c && c->isGmInvis() && !player->canSeeGmInvis(c))
					{
						CreatureVector v;
						CreatureVector::iterator it;
						Creature* notInvisCreature = NULL;
						for(it = tile->creatures.begin(); it != tile->creatures.end(); ++it){
							if(!(*it)->isGmInvis() || player->canSeeGmInvis((*it))){
								notInvisCreature = (*it);
								break;
							}
						}
 
						if(notInvisCreature){
							return notInvisCreature;
						}
						if(tile->getTopDownItem()){
							return tile->getTopDownItem();
						}
						if(tile->getTopTopItem()){
							return tile->getTopTopItem();
						}
						return tile->ground;
					}
					return c;
				}
#endif //__PB_GMINVISIBLE__
				return tile->getTopThing();
			}

			Thing* thing = NULL;

			/*for move operations*/
			if(type == STACKPOS_MOVE){
				Item* item = tile->getTopDownItem();
				if(item && !item->isNotMoveable())
					thing = item;
				else
					thing = tile->getTopCreature();
			}
			/*use item*/
			else if(type == STACKPOS_USE){
				thing = tile->getTopDownItem();
			}
			else if(type == STACKPOS_USEITEM){
				 //First check items with topOrder 2 (ladders, signs, splashes)
				Item* item =  tile->getItemByTopOrder(2);
				if(item && g_actions->hasAction(item)){
					thing = item;
				}
				else{
					//then down items
					thing = tile->getTopDownItem();
					if(thing == NULL){
						//then last we check items with topOrder 3 (doors etc)
						thing = tile->getTopTopItem();
					}
				}
			}
			else{
				thing = tile->__getThing(index);
			}

			if(player){
				//do extra checks here if the thing is accessable
				if(thing && thing->getItem()){
					if(tile->hasProperty(ISVERTICAL)){
						if(player->getPosition().x + 1 == tile->getPosition().x){
							thing = NULL;
						}
					}
					else if(tile->hasProperty(ISHORIZONTAL)){
						if(player->getPosition().y + 1 == tile->getPosition().y){
							thing = NULL;
						}
					}
				}
			}

			return thing;
		}
	}
	else{
		//container
		if(pos.y & 0x40){
			uint8_t fromCid = pos.y & 0x0F;
			uint8_t slot = pos.z;

			Container* parentcontainer = player->getContainer(fromCid);
			if(!parentcontainer)
				return NULL;

			return parentcontainer->getItem(slot);
		}
		else if(pos.y == 0 && pos.z == 0){
			const ItemType& it = Item::items.getItemIdByClientId(spriteId);
			if(it.id == 0){
				return NULL;
			}

			int32_t subType = -1;
			if(it.isFluidContainer()){
				int32_t maxFluidType = sizeof(reverseFluidMap) / sizeof(uint32_t);
				if(index < maxFluidType){
					subType = reverseFluidMap[index];
				}
			}

			return findItemOfType(player, it.id, true, subType);
		}
		//inventory
		else{
			slots_t slot = (slots_t)static_cast<unsigned char>(pos.y);
			return player->getInventoryItem(slot);
		}
	}

	return NULL;
}

void Game::internalGetPosition(Item* item, Position& pos, uint8_t& stackpos)
{
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	stackpos = 0;

	Cylinder* topParent = item->getTopParent();
	if(topParent){
		if(Player* player = dynamic_cast<Player*>(topParent)){
			pos.x = 0xFFFF;

			Container* container = dynamic_cast<Container*>(item->getParent());
			if(container){
				pos.y = ((uint16_t) ((uint16_t)0x40) | ((uint16_t)player->getContainerID(container)) );
				pos.z = container->__getIndexOfThing(item);
				stackpos = pos.z;
			}
			else{
				pos.y = player->__getIndexOfThing(item);
				stackpos = pos.y;
			}
		}
		else if(Tile* tile = topParent->getTile()){
			pos = tile->getPosition();
			stackpos = tile->__getIndexOfThing(item);
		}
	}
}

void Game::setTile(Tile* newtile)
{
	return map->setTile(newtile->getPosition(), newtile);
}

Tile* Game::getTile(uint32_t x, uint32_t y, uint32_t z)
{
	return map->getTile(x, y, z);
}

QTreeLeafNode* Game::getLeaf(uint32_t x, uint32_t y)
{
	return map->getLeaf(x, y);
}

Creature* Game::getCreatureByID(uint32_t id)
{
	if(id == 0)
		return NULL;

	AutoList<Creature>::listiterator it = listCreature.list.find(id);
	if(it != listCreature.list.end()){
		if(!(*it).second->isRemoved())
			return (*it).second;
	}

	return NULL; //just in case the player doesnt exist
}

Player* Game::getPlayerByID(uint32_t id)
{
	if(id == 0)
		return NULL;

	AutoList<Player>::listiterator it = Player::listPlayer.list.find(id);
	if(it != Player::listPlayer.list.end()) {
		if(!(*it).second->isRemoved())
			return (*it).second;
	}

	return NULL; //just in case the player doesnt exist
}

Creature* Game::getCreatureByName(const std::string& s)
{
	std::string txt1 = asUpperCaseString(s);
	for(AutoList<Creature>::listiterator it = listCreature.list.begin(); it != listCreature.list.end(); ++it){
		if(!(*it).second->isRemoved()){
			std::string txt2 = asUpperCaseString((*it).second->getName());
			if(txt1 == txt2)
				return it->second;
		}
	}

	return NULL; //just in case the creature doesnt exist
}

Player* Game::getPlayerByName(const std::string& s)
{
	std::string txt1 = asUpperCaseString(s);
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!(*it).second->isRemoved()){
			std::string txt2 = asUpperCaseString((*it).second->getName());
			if(txt1 == txt2)
				return it->second;
		}
	}

	return NULL; //just in case the player doesnt exist
}

ReturnValue Game::getPlayerByNameWildcard(const std::string& s, Player* &player)
{
	player = NULL;

	if(s.empty()){
		return RET_PLAYERWITHTHISNAMEISNOTONLINE;
	}

	if((*s.rbegin()) != '~'){
		player = getPlayerByName(s);
		if(!player){
			return RET_PLAYERWITHTHISNAMEISNOTONLINE;
		}
		return RET_NOERROR;
	}

	Player* lastFound = NULL;
	std::string txt1 = asUpperCaseString(s.substr(0, s.length()-1));

	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!(*it).second->isRemoved()){
			std::string txt2 = asUpperCaseString((*it).second->getName());
			if(txt2.substr(0, txt1.length()) == txt1){
				if(lastFound == NULL)
					lastFound = (*it).second;
				else
					return RET_NAMEISTOOAMBIGIOUS;
			}
		}
	}

	if(lastFound != NULL){
		player = lastFound;
		return RET_NOERROR;
	}

	return RET_PLAYERWITHTHISNAMEISNOTONLINE;
}

Player* Game::getPlayerByAccount(uint32_t acc)
{
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!it->second->isRemoved()){
			if(it->second->getAccount() == acc){
				return it->second;
			}
		}
	}

	return NULL;
}

PlayerVector Game::getPlayersByAccount(uint32_t acc)
{
	PlayerVector players;
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!it->second->isRemoved()){
			if(it->second->getAccount() == acc){
				players.push_back(it->second);
			}
		}
	}

	return players;
}

PlayerVector Game::getPlayersByIP(uint32_t ipadress, uint32_t mask)
{
	PlayerVector players;
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		if(!it->second->isRemoved()){
			if((it->second->getIP() & mask) == (ipadress & mask)){
				players.push_back(it->second);
			}
		}
	}

	return players;
}

bool Game::internalPlaceCreature(Creature* creature, const Position& pos, bool extendedPos /*=false*/, bool forced /*= false*/)
{
	if(creature->getParent() != NULL){
		return false;
	}

	if(!map->placeCreature(pos, creature, extendedPos, forced)){
		return false;
	}

	//std::cout << "internalPlaceCreature: " << creature << " " << creature->getID() << std::endl;

	creature->useThing2();
	creature->setID();
	listCreature.addList(creature);
	creature->addList();
	return true;
}

bool Game::placeCreature(Creature* creature, const Position& pos, bool extendedPos /*=false*/, bool forced /*= false*/)
{
	if(!internalPlaceCreature(creature, pos, extendedPos, forced)){
		return false;
	}

	SpectatorVec list;
	SpectatorVec::iterator it;
	getSpectators(list, creature->getPosition(), false, true);

	//send to client
	Player* tmpPlayer = NULL;
	for(it = list.begin(); it != list.end(); ++it) {
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendCreatureAppear(creature, true);
		}
	}

	//event method
	for(it = list.begin(); it != list.end(); ++it) {
		(*it)->onCreatureAppear(creature, true);
	}

	int32_t newStackPos = creature->getParent()->__getIndexOfThing(creature);
	creature->getParent()->postAddNotification(creature, newStackPos);

	addCreatureCheck(creature);
	checkPlayersRecord();

	creature->onPlacedCreature();
	return true;
}

bool Game::removeCreature(Creature* creature, bool isLogout /*= true*/)
{
	if(creature->isRemoved())
		return false;

#ifdef __DEBUG__
	std::cout << "removing creature "<< std::endl;
#endif

	//std::cout << "remove: " << creature << " " << creature->getID() << std::endl;

	Cylinder* cylinder = creature->getTile();

	SpectatorVec list;
	SpectatorVec::iterator it;
	getSpectators(list, cylinder->getPosition(), false, true);

	int32_t index = cylinder->__getIndexOfThing(creature);

	//send to client
	Player* player = NULL;
	for(it = list.begin(); it != list.end(); ++it){
		if((player = (*it)->getPlayer())){
			player->sendCreatureDisappear(creature, index, isLogout);
		}
	}
	
	if(!map->removeCreature(creature)){
		return false;
	} // Fix Auto-Follow Delay		

	//event method
	for(it = list.begin(); it != list.end(); ++it){
		(*it)->onCreatureDisappear(creature, index, isLogout);
	}

	creature->getParent()->postRemoveNotification(creature, index, true);

	listCreature.removeList(creature->getID());
	creature->removeList();
	creature->setRemoved();
	FreeThing(creature);

	removeCreatureCheck(creature);

	for(std::list<Creature*>::iterator cit = creature->summons.begin(); cit != creature->summons.end(); ++cit){
		(*cit)->setLossSkill(false);
		removeCreature(*cit);
	}

	creature->onRemovedCreature();
	return true;
}

bool Game::playerMoveThing(uint32_t playerId, const Position& fromPos,
	uint16_t spriteId, uint8_t fromStackPos, const Position& toPos, uint8_t count)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	uint8_t fromIndex = 0;
	if(fromPos.x == 0xFFFF){
		if(fromPos.y & 0x40){
			fromIndex = static_cast<uint8_t>(fromPos.z);
		}
		else{
			fromIndex = static_cast<uint8_t>(fromPos.y);
		}
	}
	else
		fromIndex = fromStackPos;

	Thing* thing = internalGetThing(player, fromPos, fromIndex, spriteId, STACKPOS_MOVE);
	Cylinder* toCylinder = internalGetCylinder(player, toPos);

	if(!thing || !toCylinder){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	if(Creature* movingCreature = thing->getCreature()){
		if(Position::areInRange<1,1,0>(movingCreature->getPosition(), player->getPosition())){
			SchedulerTask* task = createSchedulerTask(g_config.getNumber(ConfigManager::PUSH_INTERVAL),
				boost::bind(&Game::playerMoveCreature, this, player->getID(),
				movingCreature->getID(), movingCreature->getPosition(), toCylinder->getPosition()));
			player->setNextActionTask(task);
		}
		else{
			playerMoveCreature(playerId, movingCreature->getID(), movingCreature->getPosition(),
				toCylinder->getPosition());
		}
	}
	else if(thing->getItem()){
		playerMoveItem(playerId, fromPos, spriteId, fromStackPos, toPos, count);
	}

	return true;
}

bool Game::playerMoveCreature(uint32_t playerId, uint32_t movingCreatureId,
	const Position& movingCreatureOrigPos, const Position& toPos)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(!player->canDoAction()){
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, boost::bind(&Game::playerMoveCreature, this, playerId, movingCreatureId,
			movingCreatureOrigPos, toPos));
		player->setNextActionTask(task);
		return false;
	}

	player->setNextActionTask(NULL);

	Creature* movingCreature = getCreatureByID(movingCreatureId);

	if(!movingCreature || movingCreature->isRemoved())
		return false;

	if(!Position::areInRange<1,1,0>(movingCreatureOrigPos, player->getPosition())){
		//need to walk to the creature first before moving it
		std::list<Direction> listDir;
		if(getPathToEx(player, movingCreatureOrigPos, listDir, 0, 1, true, true)){
			Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
				this, player->getID(), listDir)));

			SchedulerTask* task = createSchedulerTask(1500, boost::bind(&Game::playerMoveCreature, this,
				playerId, movingCreatureId, movingCreatureOrigPos, toPos));
			player->setNextWalkActionTask(task);
			return true;
		}
		else{
			player->sendCancelMessage(RET_THEREISNOWAY);
			return false;
		}
	}

	Tile* toTile = map->getTile(toPos);
	const Position& movingCreaturePos = movingCreature->getPosition();

	if(!toTile){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

#ifdef __PB_GMINVISIBLE__
	if( (!movingCreature->isPushable() && !player->hasFlag(PlayerFlag_CanPushAllCreatures)) ||
		(movingCreature->isGmInvis() && !player->canSeeGmInvis(movingCreature))){
#else
	if(!movingCreature->isPushable() && !player->hasFlag(PlayerFlag_CanPushAllCreatures)){
#endif
		player->sendCancelMessage(RET_NOTMOVEABLE);
		return false;
	}

	//check throw distance
	if((std::abs(movingCreaturePos.x - toPos.x) > movingCreature->getThrowRange()) ||
			(std::abs(movingCreaturePos.y - toPos.y) > movingCreature->getThrowRange()) ||
			(std::abs(movingCreaturePos.z - toPos.z) * 4 > movingCreature->getThrowRange())){
		player->sendCancelMessage(RET_DESTINATIONOUTOFREACH);
		return false;
	}

	if(player != movingCreature){
		if(toTile->hasProperty(BLOCKPATH)){
			player->sendCancelMessage(RET_NOTENOUGHROOM);
			return false;
		}
		else if(movingCreature->getZone() == ZONE_PROTECTION && !toTile->hasFlag(TILESTATE_PROTECTIONZONE)){
			player->sendCancelMessage(RET_NOTPOSSIBLE);
			return false;
		}
	}

	ReturnValue ret = internalMoveCreature(movingCreature, movingCreature->getTile(), toTile);
	if(ret != RET_NOERROR){
		player->sendCancelMessage(ret);
		return false;
	}

	return true;
}

ReturnValue Game::internalMoveCreature(Creature* creature, Direction direction, uint32_t flags /*= 0*/)
{
	Cylinder* fromTile = creature->getTile();
	Cylinder* toTile = NULL;

	const Position& currentPos = creature->getPosition();
	Position destPos = currentPos;

	bool canChangeFloor = true;
	switch(direction){
		case NORTH:
			destPos.y -= 1;
			break;

		case SOUTH:
			destPos.y += 1;
			break;

		case WEST:
			destPos.x -= 1;
			break;

		case EAST:
			destPos.x += 1;
			break;

		case SOUTHWEST:
			destPos.x -= 1;
			destPos.y += 1;
			canChangeFloor = false;
			break;

		case NORTHWEST:
			destPos.x -= 1;
			destPos.y -= 1;
			canChangeFloor = false;
			break;

		case NORTHEAST:
			destPos.x += 1;
			destPos.y -= 1;
			canChangeFloor = false;
			break;

		case SOUTHEAST:
			destPos.x += 1;
			destPos.y += 1;
			canChangeFloor = false;
			break;
	}

	if(creature->getPlayer() && canChangeFloor){
		//try go up
		if(currentPos.z != 8 && creature->getTile()->hasHeight(3)){
			Tile* tmpTile = map->getTile(currentPos.x, currentPos.y, currentPos.z - 1);
			if(tmpTile == NULL || (tmpTile->ground == NULL && !tmpTile->hasProperty(BLOCKSOLID))){
				tmpTile = map->getTile(destPos.x, destPos.y, destPos.z - 1);
				if(tmpTile && tmpTile->ground && !tmpTile->hasProperty(BLOCKSOLID)){
					flags = flags | FLAG_IGNOREBLOCKITEM | FLAG_IGNOREBLOCKCREATURE;
					destPos.z -= 1;
				}
			}
		}
		else{
			//try go down
			Tile* tmpTile = map->getTile(destPos);
			if(currentPos.z != 7 && (tmpTile == NULL || (tmpTile->ground == NULL && !tmpTile->hasProperty(BLOCKSOLID)))){
				tmpTile = map->getTile(destPos.x, destPos.y, destPos.z + 1);

				if(tmpTile && tmpTile->hasHeight(3)){
					flags = flags | FLAG_IGNOREBLOCKITEM | FLAG_IGNOREBLOCKCREATURE;
					destPos.z += 1;
				}
			}
		}
	}

	toTile = map->getTile(destPos);

	ReturnValue ret = RET_NOTPOSSIBLE;
	if(toTile != NULL){
		ret = internalMoveCreature(creature, fromTile, toTile, flags);
	}

	if(ret != RET_NOERROR){
		if(Player* player = creature->getPlayer()){
			player->sendCancelMessage(ret);
			player->sendCancelWalk();
		}
	}

	return ret;
}

ReturnValue Game::internalMoveCreature(Creature* creature, Cylinder* fromCylinder, Cylinder* toCylinder, uint32_t flags /*= 0*/)
{
	//check if we can move the creature to the destination
	ReturnValue ret = toCylinder->__queryAdd(0, creature, 1, flags);
	if(ret != RET_NOERROR){
		return ret;
	}

	fromCylinder->getTile()->moveCreature(creature, toCylinder);

	int32_t index = 0;
	Item* toItem = NULL;
	Cylinder* subCylinder = NULL;

	uint32_t n = 0;
	while((subCylinder = toCylinder->__queryDestination(index, creature, &toItem, flags)) != toCylinder){
		toCylinder->getTile()->moveCreature(creature, subCylinder);
		toCylinder = subCylinder;
		flags = 0;

		//to prevent infinite loop
		if(++n >= MAP_MAX_LAYERS)
			break;
	}

	return RET_NOERROR;
}

bool Game::playerMoveItem(uint32_t playerId, const Position& fromPos,
	uint16_t spriteId, uint8_t fromStackPos, const Position& toPos, uint8_t count)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(!player->canDoAction()){
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, boost::bind(&Game::playerMoveItem, this,
			playerId, fromPos, spriteId, fromStackPos, toPos, count));
		player->setNextActionTask(task);
		return false;
	}

	player->setNextActionTask(NULL);

	Cylinder* fromCylinder = internalGetCylinder(player, fromPos);
	uint8_t fromIndex = 0;

	if(fromPos.x == 0xFFFF){
		if(fromPos.y & 0x40){
			fromIndex = static_cast<uint8_t>(fromPos.z);
		}
		else{
			fromIndex = static_cast<uint8_t>(fromPos.y);
		}
	}
	else
		fromIndex = fromStackPos;

	Thing* thing = internalGetThing(player, fromPos, fromIndex, spriteId, STACKPOS_MOVE);
	if(!thing || !thing->getItem()){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Item* item = thing->getItem();

	Cylinder* toCylinder = internalGetCylinder(player, toPos);
	uint8_t toIndex = 0;

	if(toPos.x == 0xFFFF){
		if(toPos.y & 0x40){
			toIndex = static_cast<uint8_t>(toPos.z);
		}
		else{
			toIndex = static_cast<uint8_t>(toPos.y);
		}
	}

	if(fromCylinder == NULL || toCylinder == NULL || item == NULL || item->getClientID() != spriteId){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	if(!item->isPushable() || item->getUniqueId() != 0){
		player->sendCancelMessage(RET_NOTMOVEABLE);
		return false;
	}

	const Position& playerPos = player->getPosition();
	const Position& mapFromPos = fromCylinder->getTile()->getPosition();
	const Position& mapToPos = toCylinder->getTile()->getPosition();

	if(playerPos.z > mapFromPos.z){
		player->sendCancelMessage(RET_FIRSTGOUPSTAIRS);
		return false;
	}

	if(playerPos.z < mapFromPos.z){
		player->sendCancelMessage(RET_FIRSTGODOWNSTAIRS);
		return false;
	}

	if(!Position::areInRange<1,1,0>(playerPos, mapFromPos)){
		//need to walk to the item first before using it
		std::list<Direction> listDir;
		if(getPathToEx(player, item->getPosition(), listDir, 0, 1, true, true)){
			Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
				this, player->getID(), listDir)));

			SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerMoveItem, this,
				playerId, fromPos, spriteId, fromStackPos, toPos, count));
			player->setNextWalkActionTask(task);
			return true;
		}
		else{
			player->sendCancelMessage(RET_THEREISNOWAY);
			return false;
		}
	}

	//hangable item specific code
	if(item->isHangable() && toCylinder->getTile()->hasProperty(SUPPORTHANGABLE)){
		//destination supports hangable objects so need to move there first

		if(toCylinder->getTile()->hasProperty(ISVERTICAL)){
			if(player->getPosition().x + 1 == mapToPos.x){
				player->sendCancelMessage(RET_NOTPOSSIBLE);
				return false;
			}
		}
		else if(toCylinder->getTile()->hasProperty(ISHORIZONTAL)){
			if(player->getPosition().y + 1 == mapToPos.y){
				player->sendCancelMessage(RET_NOTPOSSIBLE);
				return false;
			}
		}

		if(!Position::areInRange<1,1,0>(playerPos, mapToPos)){
			Position walkPos = mapToPos;
			if(toCylinder->getTile()->hasProperty(ISVERTICAL)){
				walkPos.x -= -1;
			}

			if(toCylinder->getTile()->hasProperty(ISHORIZONTAL)){
				walkPos.y -= -1;
			}

			Position itemPos = fromPos;
			uint8_t itemStackPos = fromStackPos;

			if(fromPos.x != 0xFFFF && Position::areInRange<1,1,0>(mapFromPos, player->getPosition())
				&& !Position::areInRange<1,1,0>(mapFromPos, walkPos)){
				//need to pickup the item first
				Item* moveItem = NULL;
				ReturnValue ret = internalMoveItem(fromCylinder, player, INDEX_WHEREEVER, item, count, &moveItem);
				if(ret != RET_NOERROR){
					player->sendCancelMessage(ret);
					return false;
				}

				//changing the position since its now in the inventory of the player
				internalGetPosition(moveItem, itemPos, itemStackPos);
			}

			std::list<Direction> listDir;
			if(map->getPathTo(player, walkPos, listDir)){
				Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
					this, player->getID(), listDir)));

				SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerMoveItem, this,
					playerId, itemPos, spriteId, itemStackPos, toPos, count));
				player->setNextWalkActionTask(task);
				return true;
			}
			else{
				player->sendCancelMessage(RET_THEREISNOWAY);
				return false;
			}
		}
	}

	if((std::abs(playerPos.x - mapToPos.x) > item->getThrowRange()) ||
			(std::abs(playerPos.y - mapToPos.y) > item->getThrowRange()) ||
			(std::abs(mapFromPos.z - mapToPos.z) * 4 > item->getThrowRange()) ){
		player->sendCancelMessage(RET_DESTINATIONOUTOFREACH);
		return false;
	}

	if(!canThrowObjectTo(mapFromPos, mapToPos)){
		player->sendCancelMessage(RET_CANNOTTHROW);
		return false;
	}

	ReturnValue ret = internalMoveItem(fromCylinder, toCylinder, toIndex, item, count, NULL);
	if(ret != RET_NOERROR){
		player->sendCancelMessage(ret);
		return false;
	}

	return true;
}

ReturnValue Game::internalMoveItem(Cylinder* fromCylinder, Cylinder* toCylinder,
	int32_t index, Item* item, uint32_t count, Item** _moveItem, uint32_t flags /*= 0*/)
{
	if(!toCylinder){
		return RET_NOTPOSSIBLE;
	}

	Item* toItem = NULL;

	Cylinder* subCylinder;
	int floorN = 0;
	while((subCylinder = toCylinder->__queryDestination(index, item, &toItem, flags)) != toCylinder){
		toCylinder = subCylinder;
		flags = 0;

		//to prevent infinite loop
		if(++floorN >= MAP_MAX_LAYERS)
			break;
	}

	//destination is the same as the source?
	if(item == toItem){
		return RET_NOERROR; //silently ignore move
	}

	//check if we can add this item
	ReturnValue ret = toCylinder->__queryAdd(index, item, count, flags);
	if(ret == RET_NEEDEXCHANGE){
		//check if we can add it to source cylinder
		int32_t fromIndex = fromCylinder->__getIndexOfThing(item);

		ret = fromCylinder->__queryAdd(fromIndex, toItem, toItem->getItemCount(), 0);
		if(ret == RET_NOERROR){
			//check how much we can move
			uint32_t maxExchangeQueryCount = 0;
			ReturnValue retExchangeMaxCount = fromCylinder->__queryMaxCount(-1, toItem, toItem->getItemCount(), maxExchangeQueryCount, 0);

			if(retExchangeMaxCount != RET_NOERROR && maxExchangeQueryCount == 0){
				return retExchangeMaxCount;
			}

			if((toCylinder->__queryRemove(toItem, toItem->getItemCount(), flags) == RET_NOERROR) && ret == RET_NOERROR){
				int32_t oldToItemIndex = toCylinder->__getIndexOfThing(toItem);
				toCylinder->__removeThing(toItem, toItem->getItemCount());
				fromCylinder->__addThing(toItem);

				if(oldToItemIndex != -1){
					toCylinder->postRemoveNotification(toItem, oldToItemIndex, true);
				}

				int32_t newToItemIndex = fromCylinder->__getIndexOfThing(toItem);
				if(newToItemIndex != -1){
					fromCylinder->postAddNotification(toItem, newToItemIndex);
				}

				ret = toCylinder->__queryAdd(index, item, count, flags);
				toItem = NULL;
			}
		}
	}

	if(ret != RET_NOERROR){
		return ret;
	}

	//check how much we can move
	uint32_t maxQueryCount = 0;
	ReturnValue retMaxCount = toCylinder->__queryMaxCount(index, item, count, maxQueryCount, flags);

	if(retMaxCount != RET_NOERROR && maxQueryCount == 0){
		return retMaxCount;
	}

	uint32_t m = 0;
	uint32_t n = 0;

	if(item->isStackable()){
		m = std::min((uint32_t)count, maxQueryCount);
	}
	else
		m = maxQueryCount;

	Item* moveItem = item;

	//check if we can remove this item
	ret = fromCylinder->__queryRemove(item, m, flags);
	if(ret != RET_NOERROR){
		return ret;
	}

	//remove the item
	int32_t itemIndex = fromCylinder->__getIndexOfThing(item);
	Item* updateItem = NULL;
	fromCylinder->__removeThing(item, m);
	bool isCompleteRemoval = item->isRemoved();

	//update item(s)
	if(item->isStackable()) {
		if(toItem && toItem->getID() == item->getID()){
			n = std::min((uint32_t)100 - toItem->getItemCount(), m);
			toCylinder->__updateThing(toItem, toItem->getID(), toItem->getItemCount() + n);
			updateItem = toItem;
		}

		if(m - n > 0){
			moveItem = Item::CreateItem(item->getID(), m - n);
		}
		else{
			moveItem = NULL;
		}

		if(item->isRemoved()){
			FreeThing(item);
		}
	}

	//add item
	if(moveItem /*m - n > 0*/){
		toCylinder->__addThing(index, moveItem);
	}

	if(itemIndex != -1){
		fromCylinder->postRemoveNotification(item, itemIndex, isCompleteRemoval);
	}

	if(moveItem){
		int32_t moveItemIndex = toCylinder->__getIndexOfThing(moveItem);
		if(moveItemIndex != -1){
			toCylinder->postAddNotification(moveItem, moveItemIndex);
		}
	}

	if(updateItem){
		int32_t updateItemIndex = toCylinder->__getIndexOfThing(updateItem);
		if(updateItemIndex != -1){
			toCylinder->postAddNotification(updateItem, updateItemIndex);
		}
	}

	if(_moveItem){
		if(moveItem){
			*_moveItem = moveItem;
		}
		else{
			*_moveItem = item;
		}
	}

	//we could not move all, inform the player
	if(item->isStackable() && maxQueryCount < count){
		return retMaxCount;
	}

	return ret;
}

ReturnValue Game::internalAddItem(Cylinder* toCylinder, Item* item, int32_t index /*= INDEX_WHEREEVER*/,
	uint32_t flags /*= 0*/, bool test /*= false*/)
{
	if(toCylinder == NULL || item == NULL){
		return RET_NOTPOSSIBLE;
	}

	Item* toItem = NULL;
	toCylinder = toCylinder->__queryDestination(index, item, &toItem, flags);

	//check if we can add this item
	ReturnValue ret = toCylinder->__queryAdd(index, item, item->getItemCount(), flags);
	if(ret != RET_NOERROR){
		return ret;
	}

	//check how much we can move
	uint32_t maxQueryCount = 0;
	ret = toCylinder->__queryMaxCount(index, item, item->getItemCount(), maxQueryCount, flags);

	if(ret != RET_NOERROR){
		return ret;
	}

	uint32_t m = 0;
	uint32_t n = 0;

	if(item->isStackable()){
		m = std::min((uint32_t)item->getItemCount(), maxQueryCount);
	}
	else
		m = maxQueryCount;

	if(!test){
		Item* moveItem = item;

		//update item(s)
		if(item->isStackable()) {
			if(toItem && toItem->getID() == item->getID()){
				n = std::min((uint32_t)100 - toItem->getItemCount(), m);
				toCylinder->__updateThing(toItem, toItem->getID(), toItem->getItemCount() + n);
			}

			if(m - n > 0){
				if(m - n != item->getItemCount()){
					moveItem = Item::CreateItem(item->getID(), m - n);
				}
			}
			else{
				moveItem = NULL;
				FreeThing(item);
			}
		}

		if(moveItem){
			toCylinder->__addThing(index, moveItem);

			int32_t moveItemIndex = toCylinder->__getIndexOfThing(moveItem);
			if(moveItemIndex != -1){
				toCylinder->postAddNotification(moveItem, moveItemIndex);
			}
		}
		else{
			int32_t itemIndex = toCylinder->__getIndexOfThing(item);

			if(itemIndex != -1){
				toCylinder->postAddNotification(item, itemIndex);
			}
		}
	}

	return RET_NOERROR;
}

ReturnValue Game::internalRemoveItem(Item* item, int32_t count /*= -1*/,  bool test /*= false*/, uint32_t flags /*= 0*/)
{
	Cylinder* cylinder = item->getParent();
	if(cylinder == NULL){
		return RET_NOTPOSSIBLE;
	}

	if(count == -1){
		count = item->getItemCount();
	}

	//check if we can remove this item
	ReturnValue ret = cylinder->__queryRemove(item, count, flags | FLAG_IGNORENOTMOVEABLE);
	if(ret != RET_NOERROR){
		return ret;
	}

	if(!item->canRemove()){
		return RET_NOTPOSSIBLE;
	}

	if(!test){
		int32_t index = cylinder->__getIndexOfThing(item);

		//remove the item
		cylinder->__removeThing(item, count);
		bool isCompleteRemoval = false;

		if(item->isRemoved()){
			isCompleteRemoval = true;
			FreeThing(item);
		}

		cylinder->postRemoveNotification(item, index, isCompleteRemoval);
	}

	item->onRemoved();

	return RET_NOERROR;
}

ReturnValue Game::internalPlayerAddItem(Player* player, Item* item, bool dropOnMap /*= true*/)
{
	ReturnValue ret = internalAddItem(player, item);

	if(ret != RET_NOERROR && dropOnMap){
		ret = internalAddItem(player->getTile(), item, INDEX_WHEREEVER, FLAG_NOLIMIT);
	}

	return ret;
}

Item* Game::findItemOfType(Cylinder* cylinder, uint16_t itemId,
	bool depthSearch /*= true*/, int32_t subType /*= -1*/)
{
	if(cylinder == NULL){
		return false;
	}

	std::list<Container*> listContainer;
	Container* tmpContainer = NULL;
	Thing* thing = NULL;
	Item* item = NULL;

	for(int i = cylinder->__getFirstIndex(); i < cylinder->__getLastIndex();){

		if((thing = cylinder->__getThing(i)) && (item = thing->getItem())){
			if(item->getID() == itemId && (subType == -1 || subType == item->getSubType())){
				return item;
			}
			else{
				++i;

				if(depthSearch && (tmpContainer = item->getContainer())){
					listContainer.push_back(tmpContainer);
				}
			}
		}
		else{
			++i;
		}
	}

	while(listContainer.size() > 0){
		Container* container = listContainer.front();
		listContainer.pop_front();

		for(int i = 0; i < (int32_t)container->size();){
			Item* item = container->getItem(i);
			if(item->getID() == itemId && (subType == -1 || subType == item->getSubType())){
				return item;
			}
			else{
				++i;

				if((tmpContainer = item->getContainer())){
					listContainer.push_back(tmpContainer);
				}
			}
		}
	}

	return NULL;
}

bool Game::removeItemOfType(Cylinder* cylinder, uint16_t itemId, int32_t count, int32_t subType /*= -1*/)
{
	if(cylinder == NULL || ((int32_t)cylinder->__getItemTypeCount(itemId, subType) < count) ){
		return false;
	}

	if(count <= 0){
		return true;
	}

	std::list<Container*> listContainer;
	Container* tmpContainer = NULL;
	Thing* thing = NULL;
	Item* item = NULL;

	for(int i = cylinder->__getFirstIndex(); i < cylinder->__getLastIndex() && count > 0;){

		if((thing = cylinder->__getThing(i)) && (item = thing->getItem())){
			if(item->getID() == itemId){
				if(item->isStackable()){
					if(item->getItemCount() > count){
						internalRemoveItem(item, count);
						count = 0;
					}
					else{
						count -= item->getItemCount();
						internalRemoveItem(item);
					}
				}
				else if(subType == -1 || subType == item->getSubType()){
					--count;
					internalRemoveItem(item);
				}
				else //Item has subtype but not the required one.
					++i;
			}
			else{
				++i;

				if((tmpContainer = item->getContainer())){
					listContainer.push_back(tmpContainer);
				}
			}
		}
		else{
			++i;
		}
	}

	while(listContainer.size() > 0 && count > 0){
		Container* container = listContainer.front();
		listContainer.pop_front();

		for(int i = 0; i < (int32_t)container->size() && count > 0;){
			Item* item = container->getItem(i);
			if(item->getID() == itemId){
				if(item->isStackable()){
					if(item->getItemCount() > count){
						internalRemoveItem(item, count);
						count = 0;
					}
					else{
						count-= item->getItemCount();
						internalRemoveItem(item);
					}
				}
				else if(subType == -1 || subType == item->getSubType()){
					--count;
					internalRemoveItem(item);
				}
				else //Item has subtype but not the required one.
					++i;
			}
			else{
				++i;

				if((tmpContainer = item->getContainer())){
					listContainer.push_back(tmpContainer);
				}
			}
		}
	}

	return (count == 0);
}

uint32_t Game::getMoney(const Cylinder* cylinder)
{
	if(cylinder == NULL){
		return 0;
	}

	std::list<Container*> listContainer;
	ItemList::const_iterator it;
	Container* tmpContainer = NULL;

	Thing* thing = NULL;
	Item* item = NULL;

	uint32_t moneyCount = 0;

	for(int i = cylinder->__getFirstIndex(); i < cylinder->__getLastIndex(); ++i){
		if(!(thing = cylinder->__getThing(i)))
			continue;

		if(!(item = thing->getItem()))
			continue;

		if((tmpContainer = item->getContainer())){
			listContainer.push_back(tmpContainer);
		}
		else{
			if(item->getWorth() != 0){
				moneyCount+= item->getWorth();
			}
		}
	}

	while(listContainer.size() > 0){
		Container* container = listContainer.front();
		listContainer.pop_front();

		for(it = container->getItems(); it != container->getEnd(); ++it){
			Item* item = *it;

			if((tmpContainer = item->getContainer())){
				listContainer.push_back(tmpContainer);
			}
			else if(item->getWorth() != 0){
				moneyCount+= item->getWorth();
			}
		}
	}

	return moneyCount;
}

bool Game::removeMoney(Cylinder* cylinder, int32_t money, uint32_t flags /*= 0*/)
{
	if(cylinder == NULL){
		return false;
	}
	if(money <= 0){
		return true;
	}

	std::list<Container*> listContainer;
	Container* tmpContainer = NULL;

	typedef std::multimap<int, Item*, std::less<int> > MoneyMap;
	typedef MoneyMap::value_type moneymap_pair;
	MoneyMap moneyMap;
	Thing* thing = NULL;
	Item* item = NULL;

	int32_t moneyCount = 0;

	for(int i = cylinder->__getFirstIndex(); i < cylinder->__getLastIndex() && money > 0; ++i){
		if(!(thing = cylinder->__getThing(i)))
			continue;

		if(!(item = thing->getItem()))
			continue;

		if((tmpContainer = item->getContainer())){
			listContainer.push_back(tmpContainer);
		}
		else{
			if(item->getWorth() != 0){
				moneyCount += item->getWorth();
				moneyMap.insert(moneymap_pair(item->getWorth(), item));
			}
		}
	}

	while(listContainer.size() > 0 && money > 0){
		Container* container = listContainer.front();
		listContainer.pop_front();

		for(int i = 0; i < (int32_t)container->size() && money > 0; i++){
			Item* item = container->getItem(i);

			if((tmpContainer = item->getContainer())){
				listContainer.push_back(tmpContainer);
			}
			else if(item->getWorth() != 0){
				moneyCount += item->getWorth();
				moneyMap.insert(moneymap_pair(item->getWorth(), item));
			}
		}
	}

	if(moneyCount < money){
		/*not enough money*/
		return false;
	}

	MoneyMap::iterator it2;
	for(it2 = moneyMap.begin(); it2 != moneyMap.end() && money > 0; it2++){
		Item* item = it2->second;
		internalRemoveItem(item);

		if(it2->first <= money){
			money = money - it2->first;
		}
		else{
			/* Remove a monetary value from an item*/
			int remaind = item->getWorth() - money;
			addMoney(cylinder, remaind, flags);
			money = 0;
		}

		it2->second = NULL;
	}

	moneyMap.clear();

	return (money == 0);
}

bool Game::addMoney(Cylinder* cylinder, int32_t money, uint32_t flags /*= 0*/)
{
	int crys = money / 10000;
	money -= crys * 10000;
	int plat = money / 100;
	money -= plat * 100;
	int gold = money;

	while(crys > 0){
		Item* remaindItem = Item::CreateItem(ITEM_COINS_CRYSTAL, std::min(100, crys));

		ReturnValue ret = internalAddItem(cylinder, remaindItem, INDEX_WHEREEVER, flags);
		if(ret != RET_NOERROR){
			internalAddItem(cylinder->getTile(), remaindItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
		}
		crys -= std::min(100, crys);
	}

	if(plat != 0){
		Item* remaindItem = Item::CreateItem(ITEM_COINS_PLATINUM, plat);

		ReturnValue ret = internalAddItem(cylinder, remaindItem, INDEX_WHEREEVER, flags);
		if(ret != RET_NOERROR){
			internalAddItem(cylinder->getTile(), remaindItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
		}
	}

	if(gold != 0){
		Item* remaindItem = Item::CreateItem(ITEM_COINS_GOLD, gold);

		ReturnValue ret = internalAddItem(cylinder, remaindItem, INDEX_WHEREEVER, flags);
		if(ret != RET_NOERROR){
			internalAddItem(cylinder->getTile(), remaindItem, INDEX_WHEREEVER, FLAG_NOLIMIT);
		}
	}
	return true;
}

Item* Game::transformItem(Item* item, uint16_t newId, int32_t newCount /*= -1*/)
{
	if(item->getID() == newId && (newCount == -1 || newCount == item->getSubType()))
		return item;

	Cylinder* cylinder = item->getParent();
	if(cylinder == NULL){
		return NULL;
	}

	int32_t itemIndex = cylinder->__getIndexOfThing(item);

	if(itemIndex == -1){
#ifdef __DEBUG__
		std::cout << "Error: transformItem, itemIndex == -1" << std::endl;
#endif
		return item;
	}

	if(!item->canTransform()){
		return item;
	}

	const ItemType& curType = Item::items[item->getID()];
	const ItemType& newType = Item::items[newId];

	if(curType.alwaysOnTop != newType.alwaysOnTop){
		//This only occurs when you transform items on tiles from a downItem to a topItem (or vice versa)
		//Remove the old, and add the new

		ReturnValue ret = internalRemoveItem(item);
		if(ret != RET_NOERROR){
			return item;
		}

		Item* newItem = NULL;
		if(newCount == -1){
			newItem = Item::CreateItem(newId);
		}
		else{
			newItem = Item::CreateItem(newId, newCount);
		}

		newItem->copyAttributes(item);

		ret = internalAddItem(cylinder, newItem, INDEX_WHEREEVER);
		if(ret != RET_NOERROR){
			delete newItem;
			return NULL;
		}

		return newItem;
	}

	if(curType.type == newType.type){
		//Both items has the same type so we can safely change id/subtype

		if(newCount == 0 && (item->isStackable() || item->hasCharges())){
			if(item->isStackable()){
				internalRemoveItem(item);
				return NULL;
			}
			else{
				int32_t newItemId = newId;
				if(curType.id == newType.id){
					newItemId = curType.decayTo;
				}

				if(newItemId != -1){
					item = transformItem(item, newItemId);
					return item;
				}
				else{
					internalRemoveItem(item);
					return NULL;
				}
			}
		}
		else{
			cylinder->postRemoveNotification(item, itemIndex, false);
			uint16_t itemId = item->getID();
			int32_t count = item->getSubType();

			if(curType.id != newType.id){
				if(newType.group != curType.group){
					item->setDefaultSubtype();
				}

				itemId = newId;
			}

			if(newCount != -1 && newType.hasSubType()){
				count = newCount;
			}

			cylinder->__updateThing(item, itemId, count);
			cylinder->postAddNotification(item, itemIndex);
			return item;
		}
	}
	else{
		//Replacing the the old item with the new while maintaining the old position
		Item* newItem = NULL;
		if(newCount == -1){
			newItem = Item::CreateItem(newId);
		}
		else{
			newItem = Item::CreateItem(newId, newCount);
		}
		if(newItem == NULL) {
			// Decaying into deprecated item?
		  #ifdef __DEBUG__
			std::cout << "Error: [Game::transformItem] Item of type " << item->getID() << " transforming into invalid type " << newId << std::endl;
		  #endif
			return NULL;
		}

		cylinder->__replaceThing(itemIndex, newItem);
		cylinder->postAddNotification(newItem, itemIndex);

		item->setParent(NULL);
		cylinder->postRemoveNotification(item, itemIndex, true);
		FreeThing(item);

		return newItem;
	}

	return NULL;
}

ReturnValue Game::internalTeleport(Thing* thing, const Position& newPos, uint32_t flags /*= 0*/)
{
	if(newPos == thing->getPosition())
		return RET_NOERROR;
	else if(thing->isRemoved())
		return RET_NOTPOSSIBLE;

	Tile* toTile = getTile(newPos.x, newPos.y, newPos.z);
	if(toTile){
		if(Creature* creature = thing->getCreature()){
			creature->getTile()->moveCreature(creature, toTile, true);
			return RET_NOERROR;
		}
		else if(Item* item = thing->getItem()){
			return internalMoveItem(item->getParent(), toTile, INDEX_WHEREEVER, item, item->getItemCount(), NULL, flags);
		}
	}

	return RET_NOTPOSSIBLE;
}

bool Game::anonymousBroadcastMessage(MessageClasses type, const std::string& text)
{
	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		(*it).second->sendTextMessage(type, text.c_str());
	}

	return true;
}

//Implementation of player invoked events
bool Game::playerMove(uint32_t playerId, Direction direction)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->stopWalk();
	int32_t delay = player->getWalkDelay(direction);
	if(delay > 0){
		player->setNextAction(OTSYS_TIME() + player->getStepDuration());
		SchedulerTask* task = createSchedulerTask( ((uint32_t)delay), boost::bind(&Game::playerMove, this,
			playerId, direction));
		player->setNextWalkTask(task);
		return false;
	}

	//player->setFollowCreature(NULL); -- Fix Auto-Follow Delay
	player->onWalk(direction);
	player->resetIdleTime();
	return (internalMoveCreature(player, direction) == RET_NOERROR);
}

bool Game::internalBroadcastMessage(Player* player, const std::string& text)
{
	if(!player->hasFlag(PlayerFlag_CanBroadcast))
		return false;

	for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
		(*it).second->sendCreatureSay(player, SPEAK_BROADCAST, text);
	}

	return true;
}

bool Game::playerCreatePrivateChannel(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	ChatChannel* channel = g_chat.createChannel(player, 0xFFFF);

	if(!channel){
		return false;
	}

	if(!channel->addUser(player)){
		return false;
	}

	player->sendCreatePrivateChannel(channel->getId(), channel->getName());
	return true;
}

bool Game::playerChannelInvite(uint32_t playerId, const std::string& name)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	PrivateChatChannel* channel = g_chat.getPrivateChannel(player);

	if(!channel){
		return false;
	}

	Player* invitePlayer = getPlayerByName(name);

	if(!invitePlayer){
		return false;
	}

	channel->invitePlayer(player, invitePlayer);
	return true;
}

bool Game::playerChannelExclude(uint32_t playerId, const std::string& name)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	PrivateChatChannel* channel = g_chat.getPrivateChannel(player);

	if(!channel){
		return false;
	}

	Player* excludePlayer = getPlayerByName(name);

	if(!excludePlayer){
		return false;
	}

	channel->excludePlayer(player, excludePlayer);
	return true;
}

bool Game::playerRequestChannels(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->sendChannelsDialog();
	return true;
}

bool Game::playerOpenChannel(uint32_t playerId, uint16_t channelId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(!g_chat.addUserToChannel(player, channelId)){
		return false;
	}

	ChatChannel* channel = g_chat.getChannel(player, channelId);
	if(!channel){
		return false;
	}

	if(channel->getId() != CHANNEL_RVR){
		player->sendChannel(channel->getId(), channel->getName());
	}
	else{
		// CHANNEL_RVR is the Rule Violations channel, send it properly
		player->sendRuleViolationsChannel(channel->getId());
	}

	return true;
}

bool Game::playerCloseChannel(uint32_t playerId, uint16_t channelId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	g_chat.removeUserFromChannel(player, channelId);
	return true;
}

bool Game::playerOpenPrivateChannel(uint32_t playerId, const std::string& receiver)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	uint32_t guid;
	std::string receiverName = receiver;
	if(IOPlayer::instance()->getGuidByName(guid, receiverName))
		player->sendOpenPrivateChannel(receiverName);
	else
		player->sendCancel("A player with this name does not exist.");

	return true;
}

bool Game::playerProcessRuleViolation(uint32_t playerId, const std::string& name)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(!player->hasFlag(PlayerFlag_CanAnswerRuleViolations))
		return false;

	Player* reporter = getPlayerByName(name);
	if(!reporter){
		return false;
	}

	RuleViolationsMap::iterator it = ruleViolations.find(reporter->getID());
	if(it == ruleViolations.end()){
		return false;
	}

	RuleViolation& rvr = *it->second;

	if(!rvr.isOpen){
		return false;
	}

	rvr.isOpen = false;
	rvr.gamemaster = player;

	ChatChannel* channel = g_chat.getChannelById(CHANNEL_RVR);
	if(channel){
		for(UsersMap::const_iterator it = channel->getUsers().begin(); it != channel->getUsers().end(); ++it){
			if(it->second){
				it->second->sendRemoveReport(reporter->getName());
			}
		}
	}

	return true;
}

bool Game::playerCloseRuleViolation(uint32_t playerId, const std::string& name)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Player* reporter = getPlayerByName(name);
	if(!reporter){
		return false;
	}

	return closeRuleViolation(reporter);
}

bool Game::playerCancelRuleViolation(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved()){
		return false;
	}

	return cancelRuleViolation(player);
}

bool Game::playerReceivePing(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->receivePing();
	return true;
}

bool Game::playerAutoWalk(uint32_t playerId, std::list<Direction>& listDir)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->setNextWalkTask(NULL);
	return player->startAutoWalk(listDir);
}

bool Game::playerStopAutoWalk(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->stopWalk();
	return true;
}

bool Game::playerUseItemEx(uint32_t playerId, const Position& fromPos, uint8_t fromStackPos, uint16_t fromSpriteId,
	const Position& toPos, uint8_t toStackPos, uint16_t toSpriteId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Thing* thing = internalGetThing(player, fromPos, fromStackPos, fromSpriteId, STACKPOS_USEITEM);
	if(!thing){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Item* item = thing->getItem();
	if(!item || !item->isUseable()){
		player->sendCancelMessage(RET_CANNOTUSETHISOBJECT);
		return false;
	}

	Position walkToPos = fromPos;
	ReturnValue ret = g_actions->canUse(player, fromPos);
	if(ret == RET_NOERROR){
		ret = g_actions->canUse(player, toPos, item);
		if(ret == RET_TOOFARAWAY){
			walkToPos = toPos;
		}
	}
	if(ret != RET_NOERROR){
		if(ret == RET_TOOFARAWAY){
			Position itemPos = fromPos;
			uint8_t itemStackPos = fromStackPos;

			if(fromPos.x != 0xFFFF && toPos.x != 0xFFFF && Position::areInRange<1,1,0>(fromPos, player->getPosition()) &&
				!Position::areInRange<1,1,0>(fromPos, toPos)){
				//need to pickup the item first
				Item* moveItem = NULL;
				ReturnValue ret = internalMoveItem(item->getParent(), player, INDEX_WHEREEVER,
					item, item->getItemCount(), &moveItem);
				if(ret != RET_NOERROR){
					player->sendCancelMessage(ret);
					return false;
				}

				//changing the position since its now in the inventory of the player
				internalGetPosition(moveItem, itemPos, itemStackPos);
			}

			std::list<Direction> listDir;
			if(getPathToEx(player, walkToPos, listDir, 0, 1, true, true, 10)){
				Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
					this, player->getID(), listDir)));

				SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerUseItemEx, this,
					playerId, itemPos, itemStackPos, fromSpriteId, toPos, toStackPos, toSpriteId));
				player->setNextWalkActionTask(task);
				return true;
			}
			else{
				player->sendCancelMessage(RET_THEREISNOWAY);
				return false;
			}
		}

		player->sendCancelMessage(ret);
		return false;
	}

	if(!player->canDoAction()){
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, boost::bind(&Game::playerUseItemEx, this,
			playerId, fromPos, fromStackPos, fromSpriteId, toPos, toStackPos, toSpriteId));
		player->setNextActionTask(task);
		return false;
	}

	player->setNextActionTask(NULL);

	return g_actions->useItemEx(player, fromPos, toPos, toStackPos, item);
}

bool Game::playerUseItem(uint32_t playerId, const Position& pos, uint8_t stackPos,
	uint8_t index, uint16_t spriteId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Thing* thing = internalGetThing(player, pos, stackPos, spriteId, STACKPOS_USEITEM);
	if(!thing){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Item* item = thing->getItem();
	if(!item){
		player->sendCancelMessage(RET_CANNOTUSETHISOBJECT);
		return false;
	}

	ReturnValue ret = g_actions->canUse(player, pos);
	if(ret != RET_NOERROR){
		if(ret == RET_TOOFARAWAY){
			std::list<Direction> listDir;
			if(getPathToEx(player, pos, listDir, 0, 1, true, true)){
				Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
					this, player->getID(), listDir)));

				SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerUseItem, this,
					playerId, pos, stackPos, index, spriteId));
				player->setNextWalkActionTask(task);
				return true;
			}

			ret = RET_THEREISNOWAY;
		}

		player->sendCancelMessage(ret);
		return false;
	}

	if(!player->canDoAction()){
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, boost::bind(&Game::playerUseItem, this,
			playerId, pos, stackPos, index, spriteId));
		player->setNextActionTask(task);
		return false;
	}

	player->setNextActionTask(NULL);

	g_actions->useItem(player, pos, index, item);
	return true;
}

bool Game::playerUseBattleWindow(uint32_t playerId, const Position& fromPos, uint8_t fromStackPos,
	uint32_t creatureId, uint16_t spriteId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Creature* creature = getCreatureByID(creatureId);
	if(!creature){
		return false;
	}

	if(!Position::areInRange<7,5,0>(creature->getPosition(), player->getPosition())){
		return false;
	}

	if(creature->getPlayer()){
		player->sendCancelMessage(RET_DIRECTPLAYERSHOOT);
		return false;
	}

	Thing* thing = internalGetThing(player, fromPos, fromStackPos, spriteId, STACKPOS_USE);
	if(!thing){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Item* item = thing->getItem();
	if(!item || item->getClientID() != spriteId){
		player->sendCancelMessage(RET_CANNOTUSETHISOBJECT);
		return false;
	}

	ReturnValue ret = g_actions->canUse(player, fromPos);
	if(ret != RET_NOERROR){
		if(ret == RET_TOOFARAWAY){
			std::list<Direction> listDir;
			if(getPathToEx(player, item->getPosition(), listDir, 0, 1, true, true)){
				Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
					this, player->getID(), listDir)));

				SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerUseBattleWindow, this,
					playerId, fromPos, fromStackPos, creatureId, spriteId));
				player->setNextWalkActionTask(task);
				return true;
			}

			ret = RET_THEREISNOWAY;
		}

		player->sendCancelMessage(ret);
		return false;
	}

	if(!player->canDoAction()){
		uint32_t delay = player->getNextActionTime();
		SchedulerTask* task = createSchedulerTask(delay, boost::bind(&Game::playerUseBattleWindow, this,
			playerId, fromPos, fromStackPos, creatureId, spriteId));
		player->setNextActionTask(task);
		return false;
	}

	player->setNextActionTask(NULL);

	return g_actions->useItemEx(player, fromPos, creature->getPosition(), creature->getParent()->__getIndexOfThing(creature), item, creatureId);
}

bool Game::playerCloseContainer(uint32_t playerId, uint8_t cid)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->closeContainer(cid);
	player->sendCloseContainer(cid);
	return true;
}

bool Game::playerMoveUpContainer(uint32_t playerId, uint8_t cid)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Container* container = player->getContainer(cid);
	if(!container){
		return false;
	}

	Container* parentContainer = dynamic_cast<Container*>(container->getParent());
	if(!parentContainer){
		return false;
	}

	bool hasParent = (dynamic_cast<const Container*>(parentContainer->getParent()) != NULL);
	player->addContainer(cid, parentContainer);
	player->sendContainer(cid, parentContainer, hasParent);

	return true;
}

bool Game::playerUpdateTile(uint32_t playerId, const Position& pos)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(player->canSee(pos)){
		Tile* tile = getTile(pos.x, pos.y, pos.z);
		player->sendUpdateTile(tile, pos);
		return true;
	}
	return false;
}

bool Game::playerUpdateContainer(uint32_t playerId, uint8_t cid)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Container* container = player->getContainer(cid);
	if(!container){
		return false;
	}

	bool hasParent = (dynamic_cast<const Container*>(container->getParent()) != NULL);
	player->sendContainer(cid, container, hasParent);

	return true;
}

bool Game::playerRotateItem(uint32_t playerId, const Position& pos, uint8_t stackPos, const uint16_t spriteId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Thing* thing = internalGetThing(player, pos, stackPos);
	if(!thing){
		return false;
	}

	Item* item = thing->getItem();
	if(!item || item->getClientID() != spriteId || !item->isRoteable() || item->getUniqueId() != 0){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	if(pos.x != 0xFFFF && !Position::areInRange<1,1,0>(pos, player->getPosition())){
		std::list<Direction> listDir;
		if(getPathToEx(player, pos, listDir, 0, 1, true, true)){
			Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
				this, player->getID(), listDir)));

			SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerRotateItem, this,
				playerId, pos, stackPos, spriteId));
			player->setNextWalkActionTask(task);
			return true;
		}
		else{
			player->sendCancelMessage(RET_THEREISNOWAY);
			return false;
		}
	}

	uint16_t newId = Item::items[item->getID()].rotateTo;
	if(newId != 0){
		transformItem(item, newId);
	}

	player->resetIdleTime();
	return true;
}

bool Game::playerWriteItem(uint32_t playerId, uint32_t windowTextId, const std::string& text)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	uint16_t maxTextLength = 0;
	uint32_t internalWindowTextId = 0;
	Item* writeItem = player->getWriteItem(internalWindowTextId, maxTextLength);

	if(text.length() > maxTextLength || windowTextId != internalWindowTextId){
		return false;
	}

	if(!writeItem || writeItem->isRemoved()){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Cylinder* topParent = writeItem->getTopParent();
	Player* owner = dynamic_cast<Player*>(topParent);
	if(owner && owner != player){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	if(!Position::areInRange<1,1,0>(writeItem->getPosition(), player->getPosition())){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	if(!text.empty()){
		if(writeItem->getText() != text){
			writeItem->setText(text);
			writeItem->setWriter(player->getName());
			writeItem->setWrittenDate(std::time(NULL));
		}
	}
	else{
		writeItem->resetText();
		writeItem->resetWriter();
		writeItem->resetWrittenDate();
	}

	uint16_t newId = Item::items[writeItem->getID()].writeOnceItemId;
	if(newId != 0){
		transformItem(writeItem, newId);
	}

	player->setWriteItem(NULL);
	return true;
}

bool Game::playerUpdateHouseWindow(uint32_t playerId, uint8_t listId, uint32_t windowTextId, const std::string& text)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	uint32_t internalWindowTextId;
	uint32_t internalListId;
	House* house = player->getEditHouse(internalWindowTextId, internalListId);

	if(house && internalWindowTextId == windowTextId && listId == 0){
		house->setAccessList(internalListId, text);
		player->setEditHouse(NULL);
	}

	return true;
}

bool Game::playerRequestTrade(uint32_t playerId, const Position& pos, uint8_t stackPos,
	uint32_t tradePlayerId, uint16_t spriteId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Player* tradePartner = getPlayerByID(tradePlayerId);
	if(!tradePartner || tradePartner == player) {
		player->sendTextMessage(MSG_INFO_DESCR, "Sorry, not possible.");
		return false;
	}

	if(!Position::areInRange<2,2,0>(tradePartner->getPosition(), player->getPosition())){
		std::stringstream ss;
		ss << tradePartner->getName() << " tells you to move closer.";
		player->sendTextMessage(MSG_INFO_DESCR, ss.str());
		return false;
	}

	Item* tradeItem = dynamic_cast<Item*>(internalGetThing(player, pos, stackPos, spriteId, STACKPOS_USE));
	if(!tradeItem || tradeItem->getClientID() != spriteId || !tradeItem->isPickupable() || tradeItem->getUniqueId() != 0) {
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}
	else if(player->getPosition().z > tradeItem->getPosition().z){
		player->sendCancelMessage(RET_FIRSTGOUPSTAIRS);
		return false;
	}
	else if(player->getPosition().z < tradeItem->getPosition().z){
		player->sendCancelMessage(RET_FIRSTGODOWNSTAIRS);
		return false;
	}
	else if(!Position::areInRange<1,1,0>(tradeItem->getPosition(), player->getPosition())){
		std::list<Direction> listDir;
		if(getPathToEx(player, pos, listDir, 0, 1, true, true)){
			Dispatcher::getDispatcher().addTask(createTask(boost::bind(&Game::playerAutoWalk,
				this, player->getID(), listDir)));

			SchedulerTask* task = createSchedulerTask(400, boost::bind(&Game::playerRequestTrade, this,
				playerId, pos, stackPos, tradePlayerId, spriteId));
			player->setNextWalkActionTask(task);
			return true;
		}
		else{
			player->sendCancelMessage(RET_THEREISNOWAY);
			return false;
		}
	}

	std::map<Item*, uint32_t>::const_iterator it;
	const Container* container = NULL;
	for(it = tradeItems.begin(); it != tradeItems.end(); it++) {
		if(tradeItem == it->first ||
			((container = dynamic_cast<const Container*>(tradeItem)) && container->isHoldingItem(it->first)) ||
			((container = dynamic_cast<const Container*>(it->first)) && container->isHoldingItem(tradeItem)))
		{
			player->sendTextMessage(MSG_INFO_DESCR, "This item is already being traded.");
			return false;
		}
	}

	Container* tradeContainer = tradeItem->getContainer();
	if(tradeContainer && tradeContainer->getItemHoldingCount() + 1 > 100){
		player->sendTextMessage(MSG_INFO_DESCR, "You can not trade more than 100 items.");
		return false;
	}

	return internalStartTrade(player, tradePartner, tradeItem);
}

bool Game::internalStartTrade(Player* player, Player* tradePartner, Item* tradeItem)
{
	if(player->tradeState != TRADE_NONE && !(player->tradeState == TRADE_ACKNOWLEDGE && player->tradePartner == tradePartner)) {
		player->sendCancelMessage(RET_YOUAREALREADYTRADING);
		return false;
	}
	else if(tradePartner->tradeState != TRADE_NONE && tradePartner->tradePartner != player) {
		player->sendCancelMessage(RET_THISPLAYERISALREADYTRADING);
		return false;
	}

	player->tradePartner = tradePartner;
	player->tradeItem = tradeItem;
	player->tradeState = TRADE_INITIATED;
	tradeItem->useThing2();
	tradeItems[tradeItem] = player->getID();

	player->sendTradeItemRequest(player, tradeItem, true);

	if(tradePartner->tradeState == TRADE_NONE){
		std::stringstream trademsg;
		trademsg << player->getName() <<" wants to trade with you.";
		tradePartner->sendTextMessage(MSG_INFO_DESCR, trademsg.str());
		tradePartner->tradeState = TRADE_ACKNOWLEDGE;
		tradePartner->tradePartner = player;
	}
	else{
		Item* counterOfferItem = tradePartner->tradeItem;
		player->sendTradeItemRequest(tradePartner, counterOfferItem, false);
		tradePartner->sendTradeItemRequest(player, tradeItem, false);
	}

	return true;

}

bool Game::playerAcceptTrade(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(!(player->getTradeState() == TRADE_ACKNOWLEDGE || player->getTradeState() == TRADE_INITIATED)){
		return false;
	}

	player->setTradeState(TRADE_ACCEPT);
	Player* tradePartner = player->tradePartner;
	if(tradePartner && tradePartner->getTradeState() == TRADE_ACCEPT){

		Item* tradeItem1 = player->tradeItem;
		Item* tradeItem2 = tradePartner->tradeItem;

		player->setTradeState(TRADE_TRANSFER);
		tradePartner->setTradeState(TRADE_TRANSFER);

		std::map<Item*, uint32_t>::iterator it;

		it = tradeItems.find(tradeItem1);
		if(it != tradeItems.end()) {
			FreeThing(it->first);
			tradeItems.erase(it);
		}

		it = tradeItems.find(tradeItem2);
		if(it != tradeItems.end()) {
			FreeThing(it->first);
			tradeItems.erase(it);
		}

		bool isSuccess = false;

		ReturnValue ret1 = internalAddItem(tradePartner, tradeItem1, INDEX_WHEREEVER, 0, true);
		ReturnValue ret2 = internalAddItem(player, tradeItem2, INDEX_WHEREEVER, 0, true);

		if(ret1 == RET_NOERROR && ret2 == RET_NOERROR){
			ret1 = internalRemoveItem(tradeItem1, tradeItem1->getItemCount(), true);
			ret2 = internalRemoveItem(tradeItem2, tradeItem2->getItemCount(), true);

			if(ret1 == RET_NOERROR && ret2 == RET_NOERROR){
				Cylinder* cylinder1 = tradeItem1->getParent();
				Cylinder* cylinder2 = tradeItem2->getParent();

				internalMoveItem(cylinder1, tradePartner, INDEX_WHEREEVER, tradeItem1, tradeItem1->getItemCount(), NULL);
				internalMoveItem(cylinder2, player, INDEX_WHEREEVER, tradeItem2, tradeItem2->getItemCount(), NULL);

				tradeItem1->onTradeEvent(ON_TRADE_TRANSFER, tradePartner);
				tradeItem2->onTradeEvent(ON_TRADE_TRANSFER, player);

				isSuccess = true;
			}
		}

		if(!isSuccess){
			std::string errorDescription = getTradeErrorDescription(ret1, tradeItem1);
			tradePartner->sendTextMessage(MSG_INFO_DESCR, errorDescription);
			tradePartner->tradeItem->onTradeEvent(ON_TRADE_CANCEL, tradePartner);

			errorDescription = getTradeErrorDescription(ret2, tradeItem2);
			player->sendTextMessage(MSG_INFO_DESCR, errorDescription);
			player->tradeItem->onTradeEvent(ON_TRADE_CANCEL, player);
		}

		player->setTradeState(TRADE_NONE);
		player->tradeItem = NULL;
		player->tradePartner = NULL;
		player->sendTradeClose();

		tradePartner->setTradeState(TRADE_NONE);
		tradePartner->tradeItem = NULL;
		tradePartner->tradePartner = NULL;
		tradePartner->sendTradeClose();

		return isSuccess;
	}

	return false;
}

std::string Game::getTradeErrorDescription(ReturnValue ret, Item* item)
{
	std::stringstream ss;
	if(ret == RET_NOTENOUGHCAPACITY){
		ss << "You do not have enough capacity to carry";
		if(item->isStackable() && item->getItemCount() > 1){
			ss << " these objects.";
		}
		else{
			ss << " this object." ;
		}
		ss << std::endl << " " << item->getWeightDescription();
	}
	else if(ret == RET_NOTENOUGHROOM || ret == RET_CONTAINERNOTENOUGHROOM){
		ss << "You do not have enough room to carry";
		if(item->isStackable() && item->getItemCount() > 1){
			ss << " these objects.";
		}
		else{
			ss << " this object.";
		}
	}
	else{
		ss << "Trade could not be completed.";
	}
	return ss.str().c_str();
}

bool Game::playerLookInTrade(uint32_t playerId, bool lookAtCounterOffer, int index)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Player* tradePartner = player->tradePartner;
	if(!tradePartner)
		return false;

	Item* tradeItem = NULL;

	if(lookAtCounterOffer)
		tradeItem = tradePartner->getTradeItem();
	else
		tradeItem = player->getTradeItem();

	if(!tradeItem)
		return false;

	int32_t lookDistance = std::max(std::abs(player->getPosition().x - tradeItem->getPosition().x),
		std::abs(player->getPosition().y - tradeItem->getPosition().y));

	if(index == 0){
		std::stringstream ss;
		ss << "You see " << tradeItem->getDescription(lookDistance);
		player->sendTextMessage(MSG_INFO_DESCR, ss.str());
		return false;
	}

	Container* tradeContainer = tradeItem->getContainer();
	if(!tradeContainer || index > (int)tradeContainer->getItemHoldingCount())
		return false;

	bool foundItem = false;
	std::list<const Container*> listContainer;
	ItemList::const_iterator it;
	Container* tmpContainer = NULL;

	listContainer.push_back(tradeContainer);

	while(!foundItem && listContainer.size() > 0){
		const Container* container = listContainer.front();
		listContainer.pop_front();

		for(it = container->getItems(); it != container->getEnd(); ++it){
			if((tmpContainer = (*it)->getContainer())){
				listContainer.push_back(tmpContainer);
			}

			--index;
			if(index == 0){
				tradeItem = *it;
				foundItem = true;
				break;
			}
		}
	}

	if(foundItem){
		std::stringstream ss;
		ss << "You see " << tradeItem->getDescription(lookDistance);
		player->sendTextMessage(MSG_INFO_DESCR, ss.str());
	}

	return foundItem;
}

bool Game::playerCloseTrade(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	return internalCloseTrade(player);
}

bool Game::internalCloseTrade(Player* player)
{
	Player* tradePartner = player->tradePartner;
	if((tradePartner && tradePartner->getTradeState() == TRADE_TRANSFER) ||
		  player->getTradeState() == TRADE_TRANSFER){
  		std::cout << "Warning: [Game::playerCloseTrade] TradeState == TRADE_TRANSFER. " <<
		  	player->getName() << " " << player->getTradeState() << " , " <<
		  	tradePartner->getName() << " " << tradePartner->getTradeState() << std::endl;
		return true;
	}

	std::vector<Item*>::iterator it;
	if(player->getTradeItem()){
		std::map<Item*, uint32_t>::iterator it = tradeItems.find(player->getTradeItem());
		if(it != tradeItems.end()) {
			FreeThing(it->first);
			tradeItems.erase(it);
		}

		player->tradeItem->onTradeEvent(ON_TRADE_CANCEL, player);
		player->tradeItem = NULL;
	}

	player->setTradeState(TRADE_NONE);
	player->tradePartner = NULL;

	player->sendTextMessage(MSG_STATUS_SMALL, "Trade cancelled.");
	player->sendTradeClose();

	if(tradePartner){
		if(tradePartner->getTradeItem()){
			std::map<Item*, uint32_t>::iterator it = tradeItems.find(tradePartner->getTradeItem());
			if(it != tradeItems.end()) {
				FreeThing(it->first);
				tradeItems.erase(it);
			}

			tradePartner->tradeItem->onTradeEvent(ON_TRADE_CANCEL, tradePartner);
			tradePartner->tradeItem = NULL;
		}

		tradePartner->setTradeState(TRADE_NONE);
		tradePartner->tradePartner = NULL;

		tradePartner->sendTextMessage(MSG_STATUS_SMALL, "Trade cancelled.");
		tradePartner->sendTradeClose();
	}

	return true;
}

bool Game::playerLookAt(uint32_t playerId, const Position& pos, uint16_t spriteId, uint8_t stackPos)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	Thing* thing = internalGetThing(player, pos, stackPos, spriteId, STACKPOS_LOOK);
	if(!thing){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Position thingPos = thing->getPosition();
	if(!player->canSee(thingPos)){
		player->sendCancelMessage(RET_NOTPOSSIBLE);
		return false;
	}

	Position playerPos = player->getPosition();

	int32_t lookDistance = 0;
	if(thing == player)
		lookDistance = -1;
	else{
		lookDistance = std::max(std::abs(playerPos.x - thingPos.x), std::abs(playerPos.y - thingPos.y));
		if(playerPos.z != thingPos.z)
			lookDistance = lookDistance + 9 + 6;
	}

	std::stringstream ss;
	ss << "You see " << thing->getDescription(lookDistance) << std::endl;
	
	if(player->getAccessLevel())
	{
		Item* item = thing->getItem();
		if(item)
		{
			ss << "ItemID: [" << item->getID() << "].";
			if(item->getActionId() > 0)
				ss << std::endl << "ActionID: [" << item->getActionId() << "].";
			if(item->getUniqueId() > 0)
				ss << std::endl << "UniqueID: [" << item->getUniqueId() << "].";
			ss << std::endl;
		}
        else if(Creature* creature = thing->getCreature())
		{
			ss << "Health: [" << creature->getHealth() << "/" << creature->getMaxHealth() << "]." << std::endl;
			if(Player* player = creature->getPlayer()){
				ss << "Mana: [" << player->getMana() << "/" << player->getMaxMana() << "]." << std::endl;

					char buffer[32];
					formatIP(player->getIP(), buffer);
					ss << "IP: [" << buffer << "]." << std::endl;
			}
		}
		ss << "Position: [X: " << thingPos.x << "] [Y: " << thingPos.y << "] [Z: " << thingPos.z << "].";
    }

	player->sendTextMessage(MSG_INFO_DESCR, ss.str());

	player->resetIdleTime();
	return true;
}

bool Game::playerCancelAttackAndFollow(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	playerSetAttackedCreature(playerId, 0);
	playerFollowCreature(playerId, 0);
	player->stopWalk();
	return true;
}

bool Game::playerSetAttackedCreature(uint32_t playerId, uint32_t creatureId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	if(player->getAttackedCreature() && creatureId == 0){
		player->setAttackedCreature(NULL);
		player->sendCancelTarget();
		return true;
	}

	Creature* attackCreature = getCreatureByID(creatureId);
	if(!attackCreature){
		player->setAttackedCreature(NULL);
		player->sendCancelTarget();
		return false;
	}

	ReturnValue ret = Combat::canTargetCreature(player, attackCreature);
	if(ret != RET_NOERROR){
		player->sendCancelMessage(ret);
		player->sendCancelTarget();
		player->setAttackedCreature(NULL);
		return false;
	}

	player->setAttackedCreature(attackCreature);
	return true;
}

bool Game::playerFollowCreature(uint32_t playerId, uint32_t creatureId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->setAttackedCreature(NULL);
	Creature* followCreature = NULL;

	if(creatureId != 0){
		followCreature = getCreatureByID(creatureId);
	}

	return player->setFollowCreature(followCreature);
}

bool Game::playerInviteToParty(uint32_t playerId, uint32_t invitedId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved()){
		return false;
	}

	Player* invitedPlayer = getPlayerByID(invitedId);
	if(!invitedPlayer || invitedPlayer->isRemoved() || invitedPlayer->isInviting(player)){
		return false;
	}

	if(invitedPlayer->getParty()){
		std::stringstream ss;
		ss << invitedPlayer->getName() << " is already in a party.";
		player->sendTextMessage(MSG_INFO_DESCR, ss.str());
		return false;
	}

	Party* party = player->getParty();
	if(!party){
		party = new Party(player);
	}
	else if(party->getLeader() != player){
		return false;
	}

	return party->invitePlayer(invitedPlayer);
}

bool Game::playerJoinParty(uint32_t playerId, uint32_t leaderId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved()){
		return false;
	}

	Player* leader = getPlayerByID(leaderId);
	if(!leader || leader->isRemoved() || !leader->isInviting(player)){
		return false;
	}

	if(!leader->getParty() || leader->getParty()->getLeader() != leader){
		return false;
	}

	if(player->getParty()){
		player->sendTextMessage(MSG_INFO_DESCR, "You are already in a party.");
		return false;
	}

	return leader->getParty()->joinParty(player);
}

bool Game::playerRevokePartyInvitation(uint32_t playerId, uint32_t invitedId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved() || !player->getParty() || player->getParty()->getLeader() != player){
		return false;
	}

	Player* invitedPlayer = getPlayerByID(invitedId);
	if(!invitedPlayer || invitedPlayer->isRemoved() || !player->isInviting(invitedPlayer)){
		return false;
	}

	return player->getParty()->revokeInvitation(invitedPlayer);
}

bool Game::playerPassPartyLeadership(uint32_t playerId, uint32_t newLeaderId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved() || !player->getParty() || player->getParty()->getLeader() != player){
		return false;
	}

	Player* newLeader = getPlayerByID(newLeaderId);
	if(!newLeader || newLeader->isRemoved() || !player->isPartner(newLeader)){
		return false;
	}

	return player->getParty()->passPartyLeadership(newLeader);
}

bool Game::playerLeaveParty(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved()){
		return false;
	}

	if(!player->getParty() || player->hasCondition(CONDITION_INFIGHT)){
		return false;
	}

	return player->getParty()->leaveParty(player);
}

bool Game::playerSetFightModes(uint32_t playerId, fightMode_t fightMode, chaseMode_t chaseMode, bool safeMode)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->setFightMode(fightMode);
	player->setChaseMode(chaseMode);
	player->setSafeMode(safeMode);
	return true;
}

bool Game::playerRequestAddVip(uint32_t playerId, const std::string& vip_name)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	std::string real_name;
	real_name = vip_name;
	uint32_t guid;
	bool specialVip;

	if(!IOPlayer::instance()->getGuidByNameEx(guid, specialVip, real_name)){
		player->sendTextMessage(MSG_STATUS_SMALL, "A player with that name does not exist.");
		return false;
	}
	if(specialVip && !player->hasFlag(PlayerFlag_SpecialVIP)){
		player->sendTextMessage(MSG_STATUS_SMALL, "You can not add this player.");
		return false;
	}

	bool online = (getPlayerByName(real_name) != NULL);
	return player->addVIP(guid, real_name, online);
}

bool Game::playerRequestRemoveVip(uint32_t playerId, uint32_t guid)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->removeVIP(guid);
	return true;
}

bool Game::playerTurn(uint32_t playerId, Direction dir)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->resetIdleTime();
	return internalCreatureTurn(player, dir);
}

bool Game::playerRequestOutfit(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->sendOutfitWindow(player);
	return true;
}

bool Game::playerChangeOutfit(uint32_t playerId, Outfit_t outfit)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->defaultOutfit = outfit;
	
	if(player->hasCondition(CONDITION_OUTFIT)){
		return false;
	}
	
	internalCreatureChangeOutfit(player, outfit);
	return true;
}

bool Game::playerSay(uint32_t playerId, uint16_t channelId, SpeakClasses type,
	const std::string& receiver, const std::string& text)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	uint32_t muteTime = player->isMuted();
	if(muteTime > 0){
		std::stringstream ss;
		ss << "You are still muted for " << muteTime << " seconds.";
		player->sendTextMessage(MSG_STATUS_SMALL, ss.str());
		return false;
	}


	TalkActionResult_t result;
	result = g_talkactions->onPlayerSpeak(player, type, text);
	if(result == TALKACTION_BREAK){
		return true;
	}

	if(playerSayCommand(player, type, text)){
		return true;
	}

	if(playerSaySpell(player, type, text)){
		return true;
	}

	player->removeMessageBuffer();
	player->resetIdleTime();

	switch(type){
		case SPEAK_SAY:
			return internalCreatureSay(player, SPEAK_SAY, text);
			break;
		case SPEAK_WHISPER:
			return playerWhisper(player, text);
			break;
		case SPEAK_YELL:
			return playerYell(player, text);
			break;
		case SPEAK_PRIVATE:
		case SPEAK_PRIVATE_RED:
		case SPEAK_RVR_ANSWER:
			return playerSpeakTo(player, type, receiver, text);
			break;
		case SPEAK_CHANNEL_Y:
		case SPEAK_CHANNEL_R1:
		case SPEAK_CHANNEL_R2:
			return playerTalkToChannel(player, type, text, channelId);
			break;
		case SPEAK_BROADCAST:
			return internalBroadcastMessage(player, text);
			break;
		case SPEAK_RVR_CHANNEL:
			return playerReportRuleViolation(player, text);
			break;
		case SPEAK_RVR_CONTINUE:
			return playerContinueReport(player, text);
			break;

		default:
			break;
	}

	return false;
}

bool Game::playerSayCommand(Player* player, SpeakClasses type, const std::string& text)
{
	//First, check if this was a command
	for(uint32_t i = 0; i < commandTags.size(); i++){
		if(commandTags[i] == text.substr(0,1)){
			if(commands.exeCommand(player, text) || player->getAccessLevel() > 0){
				return true;
			}
		}
	}

	return false;
}

bool Game::playerSaySpell(Player* player, SpeakClasses type, const std::string& text)
{
	TalkActionResult_t result;
	result = g_spells->playerSaySpell(player, type, text);
	if(result == TALKACTION_BREAK){
        if(g_config.getBool(ConfigManager::SPELLS_ORANGE))
            return internalCreatureSay(player, SPEAK_MONSTER_SAY, text);
		else
            return internalCreatureSay(player, SPEAK_SAY, text);
	}
	else if(result == TALKACTION_FAILED){
		return true;
	}

	return false;
}

bool Game::playerWhisper(Player* player, const std::string& text)
{
	SpectatorVec list;
	SpectatorVec::iterator it;
	getSpectators(list, player->getPosition(), false, false,
		Map::maxClientViewportX, Map::maxClientViewportX,
		Map::maxClientViewportY, Map::maxClientViewportY);

	//send to client
	Player* tmpPlayer = NULL;
	for(it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			if(!Position::areInRange<1,1,0>(player->getPosition(), (*it)->getPosition())){
				tmpPlayer->sendCreatureSay(player, SPEAK_WHISPER, "pspsps");
			}
			else{
				tmpPlayer->sendCreatureSay(player, SPEAK_WHISPER, text);
			}
		}
	}

	//event method
	for(it = list.begin(); it != list.end(); ++it) {
		(*it)->onCreatureSay(player, SPEAK_WHISPER, text);
	}

	return true;
}

bool Game::playerYell(Player* player, const std::string& text)
{
	int32_t addExhaustion = 0;
	bool isExhausted = false;
	if(!player->hasCondition(CONDITION_EXHAUSTED)){
		addExhaustion = g_config.getNumber(ConfigManager::EXHAUSTED);
		internalCreatureSay(player, SPEAK_YELL, asUpperCaseString(text));
	}
	else{
		isExhausted = true;
		addExhaustion = g_config.getNumber(ConfigManager::EXHAUSTED_ADD);
		player->sendTextMessage(MSG_STATUS_SMALL, "You are exhausted.");
	}

	if(addExhaustion > 0){
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_EXHAUSTED, addExhaustion, 0);
		player->addCondition(condition);
	}

	return !isExhausted;
}

bool Game::playerSpeakTo(Player* player, SpeakClasses type, const std::string& receiver,
	const std::string& text)
{
	Player* toPlayer = getPlayerByName(receiver);
	if(!toPlayer || player->isRemoved()) {
		player->sendTextMessage(MSG_STATUS_SMALL, "A player with this name is not online.");
		return false;
	}

	if(type == SPEAK_PRIVATE_RED && !player->hasFlag(PlayerFlag_CanTalkRedPrivate)){
		type = SPEAK_PRIVATE;
	}

	toPlayer->sendCreatureSay(player, type, text);
	toPlayer->onCreatureSay(player, type, text);

	std::stringstream ss;
	ss << "Message sent to " << toPlayer->getName() << ".";
	player->sendTextMessage(MSG_STATUS_SMALL, ss.str());
	return true;
}

bool Game::playerTalkToChannel(Player* player, SpeakClasses type, const std::string& text, unsigned short channelId)
{
	if(type == SPEAK_CHANNEL_Y && player->hasFlag(PlayerFlag_CanTalkRedChannel)){
		type = SPEAK_CHANNEL_R1;
	}
	else if(type == SPEAK_CHANNEL_Y && player->hasFlag(PlayerFlag_CanTalkRedChannelAnonymous)){
		type = SPEAK_CHANNEL_R2;
	}

	g_chat.talkToChannel(player, type, text, channelId);
	return true;
}

bool Game::playerReportRuleViolation(Player* player, const std::string& text)
{
	//Do not allow reports on multiclones worlds
	//Since reports are name-based
	if(g_config.getNumber(ConfigManager::ALLOW_CLONES)){
		player->sendTextMessage(MSG_INFO_DESCR, "Rule violations reports are disabled.");
		return false;
	}

	cancelRuleViolation(player);

	shared_ptr<RuleViolation> rvr(new RuleViolation(
		player,
		text,
		std::time(NULL)
		));

	ruleViolations[player->getID()] = rvr;

	ChatChannel* channel = g_chat.getChannelById(CHANNEL_RVR); //Rule Violations channel
	if(channel){
		channel->talk(player, SPEAK_RVR_CHANNEL, text, rvr->time);
		return true;
	}
	return false;
}

bool Game::playerContinueReport(Player* player, const std::string& text)
{
	RuleViolationsMap::iterator it = ruleViolations.find(player->getID());
	if(it == ruleViolations.end()){
		return false;
	}

	RuleViolation& rvr = *it->second;
	Player* toPlayer = rvr.gamemaster;
	if(!toPlayer){
		return false;
	}

	toPlayer->sendCreatureSay(player, SPEAK_RVR_CONTINUE, text);

	player->sendTextMessage(MSG_STATUS_SMALL, "Message sent to Gamemaster.");
	return true;
}

bool Game::kickPlayer(uint32_t playerId)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved())
		return false;

	player->kickPlayer();
	return true;
}

//--
bool Game::canThrowObjectTo(const Position& fromPos, const Position& toPos, bool checkLineOfSight /*= true*/,
	int32_t rangex /*= Map::maxClientViewportX*/, int32_t rangey /*= Map::maxClientViewportY*/)
{
	return map->canThrowObjectTo(fromPos, toPos, checkLineOfSight, rangex, rangey);
}

bool Game::isSightClear(const Position& fromPos, const Position& toPos, bool floorCheck)
{
	return map->isSightClear(fromPos, toPos, floorCheck);
}

bool Game::internalCreatureTurn(Creature* creature, Direction dir)
{
	if(creature->getDirection() != dir){
		creature->setDirection(dir);

		int32_t stackpos = creature->getParent()->__getIndexOfThing(creature);

		const SpectatorVec& list = getSpectators(creature->getPosition());
		SpectatorVec::const_iterator it;

		//send to client
		Player* tmpPlayer = NULL;
		for(it = list.begin(); it != list.end(); ++it) {
			if((tmpPlayer = (*it)->getPlayer())){
#ifdef __PB_GMINVISIBLE__
				if(!creature->isGmInvis() || tmpPlayer->canSeeGmInvis(creature)){
					int32_t i = 0;
					Tile* t = creature->getTile();
					for(CreatureVector::iterator it = t->creatures.begin(); it != t->creatures.end(); ++it){
						int32_t itIndex = t->__getIndexOfThing((*it));
						if(itIndex < stackpos){
							if((*it)->isGmInvis() && !tmpPlayer->canSeeGmInvis((*it))){
								i++; //There is one invisible gm under the creature
							}
						}
					}
 
					tmpPlayer->sendCreatureTurn(creature, stackpos-i);
				}
#else
				tmpPlayer->sendCreatureTurn(creature, stackpos);
#endif
			}
		}

		//event method
		for(it = list.begin(); it != list.end(); ++it) {
			(*it)->onCreatureTurn(creature, stackpos);
		}

		return true;
	}

	return false;
}

bool Game::internalCreatureSay(Creature* creature, SpeakClasses type, const std::string& text)
{
	// This somewhat complex construct ensures that the cached SpectatorVec
	// is used if available and if it can be used, else a local vector is
	// used. (Hopefully the compiler will optimize away the construction of
	// the temporary when it's not used.
	SpectatorVec list;
	SpectatorVec::const_iterator it;

	if(type == SPEAK_YELL || type == SPEAK_MONSTER_YELL){
		getSpectators(list, creature->getPosition(), false, true, 18, 18, 14, 14);
	}
	else{
		getSpectators(list, creature->getPosition(), false, false,
			Map::maxClientViewportX, Map::maxClientViewportX,
			Map::maxClientViewportY, Map::maxClientViewportY);
	}

	//send to client
	Player* tmpPlayer = NULL;
	for(it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendCreatureSay(creature, type, text);
		}
	}

	//event method
	for(it = list.begin(); it != list.end(); ++it){
		(*it)->onCreatureSay(creature, type, text);
	}
}

bool Game::getPathTo(const Creature* creature, const Position& destPos,
	std::list<Direction>& listDir, int32_t maxSearchDist /*= -1*/)
{
	return map->getPathTo(creature, destPos, listDir, maxSearchDist);
}

bool Game::getPathToEx(const Creature* creature, const Position& targetPos,
	std::list<Direction>& dirList, const FindPathParams& fpp)
{
	return map->getPathMatching(creature, dirList, FrozenPathingConditionCall(targetPos), fpp);
}

bool Game::getPathToEx(const Creature* creature, const Position& targetPos, std::list<Direction>& dirList,
	uint32_t minTargetDist, uint32_t maxTargetDist, bool fullPathSearch /*= true*/,
	bool clearSight /*= true*/, int32_t maxSearchDist /*= -1*/)
{
	FindPathParams fpp;
	fpp.fullPathSearch = fullPathSearch;
	fpp.maxSearchDist = maxSearchDist;
	fpp.clearSight = clearSight;
	fpp.minTargetDist = minTargetDist;
	fpp.maxTargetDist = maxTargetDist;

	return getPathToEx(creature, targetPos, dirList, fpp);
}

void Game::checkCreatureWalk(uint32_t creatureId)
{
	Creature* creature = getCreatureByID(creatureId);
	if(creature && creature->getHealth() > 0){
		creature->onWalk();
		cleanup();
	}
}

void Game::updateCreatureWalk(uint32_t creatureId)
{
	Creature* creature = getCreatureByID(creatureId);
	if(creature && creature->getHealth() > 0){
		creature->getPathToFollowCreature();
	}
}

void Game::checkCreatureAttack(uint32_t creatureId)
{
	Creature* creature = getCreatureByID(creatureId);
	if(creature && creature->getHealth() > 0){
		creature->onAttacking(0);
	}
}

void Game::addCreatureCheck(Creature* creature)
{
	if(creature->checkCreatureVectorIndex != 0) {
		// Already in a vector
		return;
	}
	int next_vector = (checkCreatureLastIndex + 1) % EVENT_CREATURECOUNT;
	checkCreatureVectors[next_vector].push_back(creature);
	creature->checkCreatureVectorIndex = next_vector + 1;
}

void Game::removeCreatureCheck(Creature* creature)
{
	if(creature->checkCreatureVectorIndex == 0) {
		// Not in any vector
		return;
	}
	std::vector<Creature*>& checkCreatureVector = checkCreatureVectors[creature->checkCreatureVectorIndex - 1];

	std::vector<Creature*>::iterator cit = std::find(checkCreatureVector.begin(),
	checkCreatureVector.end(), creature);
	if(cit != checkCreatureVector.end()) {
		// Swap & pop is more effective than erase
		std::swap(*cit, checkCreatureVector.back());
		checkCreatureVector.pop_back();
	}
	creature->checkCreatureVectorIndex = 0;
}

void Game::checkCreatures()
{
	Scheduler::getScheduler().addEvent(createSchedulerTask(
		EVENT_CHECK_CREATURE_INTERVAL, boost::bind(&Game::checkCreatures, this)));

	checkCreatureLastIndex++;
	if(checkCreatureLastIndex == EVENT_CREATURECOUNT) {
		checkCreatureLastIndex = 0;
	}

	std::vector<Creature*>& checkCreatureVector = checkCreatureVectors[checkCreatureLastIndex];

	Creature* creature;
	for(uint32_t i = 0; i < checkCreatureVector.size(); ++i){
		creature = checkCreatureVector[i];
		if(creature->getHealth() > 0){
			creature->onThink(EVENT_CREATURE_THINK_INTERVAL);
			creature->onAttacking(EVENT_CREATURE_THINK_INTERVAL);
			creature->executeConditions(EVENT_CREATURE_THINK_INTERVAL);
		}
		else if(!creature->isDying){
			creature->isDying = true;
			uint16_t dd = random_range(150, 400);
			Scheduler::getScheduler().addEvent(createSchedulerTask(dd, boost::bind(&Game::doDeathDelay, this, creature)));
		}
	}
	cleanup();
}

void Game::changeSpeed(Creature* creature, int32_t varSpeedDelta)
{
	int32_t varSpeed = creature->getSpeed() - creature->getBaseSpeed();
	varSpeed += varSpeedDelta;

	creature->setSpeed(varSpeed);

	const SpectatorVec& list = getSpectators(creature->getPosition());
	SpectatorVec::const_iterator it;

	//send to client
	Player* tmpPlayer = NULL;
	for(it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendChangeSpeed(creature, creature->getStepSpeed());
		}
	}
}

void Game::internalCreatureChangeOutfit(Creature* creature, const Outfit_t& outfit)
{
	creature->setCurrentOutfit(outfit);

	if(!creature->isInvisible()){
		SpectatorVec list;
		SpectatorVec::iterator it;
		getSpectators(list, creature->getPosition(), true);

		//send to client
		Player* tmpPlayer = NULL;
		for(it = list.begin(); it != list.end(); ++it) {
			if((tmpPlayer = (*it)->getPlayer())){
				tmpPlayer->sendCreatureChangeOutfit(creature, outfit);
			}
		}

		//event method
		for(it = list.begin(); it != list.end(); ++it) {
			(*it)->onCreatureChangeOutfit(creature, outfit);
		}
	}
}

void Game::internalCreatureChangeVisible(Creature* creature, bool visible)
{
	const SpectatorVec& list = getSpectators(creature->getPosition());
	SpectatorVec::const_iterator it;

	//send to client
	Player* tmpPlayer = NULL;
	for(it = list.begin(); it != list.end(); ++it) {
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendCreatureChangeVisible(creature, visible);
		}
	}

	//event method
	for(it = list.begin(); it != list.end(); ++it) {
		(*it)->onCreatureChangeVisible(creature, visible);
	}
}

void Game::changeLight(const Creature* creature)
{
	const SpectatorVec& list = getSpectators(creature->getPosition());

	//send to client
	Player* tmpPlayer = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendCreatureLight(creature);
		}
	}
}

bool Game::combatBlockHit(CombatType_t combatType, Creature* attacker, Creature* target,
	int32_t& healthChange, bool checkDefense, bool checkArmor)
{
	if(healthChange > 0){
		return false;
	}

	const Position& targetPos = target->getPosition();

	const SpectatorVec& list = getSpectators(targetPos);

	if(!target->isAttackable() || Combat::canDoCombat(attacker, target) != RET_NOERROR){
		addMagicEffect(list, targetPos, NM_ME_PUFF);
		return true;
	}

	int32_t damage = -healthChange;
	BlockType_t blockType = target->blockHit(attacker, combatType, damage, checkDefense, checkArmor);
	healthChange = -damage;

	if(blockType == BLOCK_DEFENSE){
		addMagicEffect(list, targetPos, NM_ME_PUFF);
		return true;
	}
	else if(blockType == BLOCK_ARMOR){
		addMagicEffect(list, targetPos, NM_ME_BLOCKHIT);
		return true;
	}
	else if(blockType == BLOCK_IMMUNITY){
		uint8_t hitEffect = 0;

		switch(combatType){
			case COMBAT_UNDEFINEDDAMAGE:
				break;

			case COMBAT_ENERGYDAMAGE:
			case COMBAT_FIREDAMAGE:
			case COMBAT_PHYSICALDAMAGE:
			{
				hitEffect = NM_ME_BLOCKHIT;
				break;
			}

			case COMBAT_POISONDAMAGE:
			{
				hitEffect = NM_ME_POISON_RINGS;
				break;
			}


			default:
				hitEffect = NM_ME_PUFF;
				break;
		}

		addMagicEffect(list, targetPos, hitEffect);

		return true;
	}

	return false;
}

bool Game::combatChangeHealth(CombatType_t combatType, Creature* attacker, Creature* target, int32_t healthChange)
{
	return combatChangeHealth(combatType, NM_ME_UNK, TEXTCOLOR_UNK, attacker, target, healthChange);
}

bool Game::combatChangeHealth(CombatType_t combatType, MagicEffectClasses customHitEffect, 
        TextColor_t customTextColor, Creature* attacker, Creature* target, int32_t healthChange)
{
	const Position& targetPos = target->getPosition();

	if(healthChange > 0){
		if(target->getHealth() <= 0){
			return false;
		}

		target->gainHealth(attacker, healthChange);
	}
	else{
		const SpectatorVec& list = getSpectators(targetPos);

		if(!target->isAttackable() || Combat::canDoCombat(attacker, target) != RET_NOERROR){
			addMagicEffect(list, targetPos, NM_ME_PUFF);
			return true;
		}

		int32_t damage = -healthChange;
		if(damage != 0){
			if(target->hasCondition(CONDITION_MANASHIELD) && combatType != COMBAT_UNDEFINEDDAMAGE){
				int32_t manaDamage = std::min(target->getMana(), damage);
				damage = std::max((int32_t)0, damage - manaDamage);

				if(manaDamage != 0){
					target->drainMana(attacker, manaDamage);

					std::stringstream ss;
					ss << manaDamage;
					addMagicEffect(list, targetPos, NM_ME_LOSE_ENERGY);
					addAnimatedText(list, targetPos, TEXTCOLOR_BLUE, ss.str());
				}
			}

			damage = std::min(target->getHealth(), damage);
			if(damage > 0){
				target->drainHealth(attacker, combatType, damage);
				addCreatureHealth(list, target);
				if(Player* player = target->getPlayer()) {
					if(player->getHealth() <= 0)
						player->sendTextMessage(MSG_EVENT_ADVANCE, "You are dead.");
				}

				TextColor_t textColor = TEXTCOLOR_NONE;
				uint8_t hitEffect = 0;

				switch(combatType){
					case COMBAT_PHYSICALDAMAGE:
					{
						Item* splash = NULL;
						switch(target->getRace()){
							case RACE_VENOM:
								textColor = TEXTCOLOR_LIGHTGREEN;
								hitEffect = NM_ME_POISON;
								splash = Item::CreateItem(ITEM_SMALLSPLASH, FLUID_GREEN);
								break;

							case RACE_BLOOD:
								textColor = TEXTCOLOR_RED;
								hitEffect = NM_ME_DRAW_BLOOD;
								splash = Item::CreateItem(ITEM_SMALLSPLASH, FLUID_BLOOD);
								break;

							case RACE_UNDEAD:
								textColor = TEXTCOLOR_LIGHTGREY;
								hitEffect = NM_ME_HIT_AREA;
								break;

							case RACE_FIRE:
								textColor = TEXTCOLOR_ORANGE;
								hitEffect = NM_ME_DRAW_BLOOD;
								break;

							default:
								break;
						}

						if(splash){
							internalAddItem(target->getTile(), splash, INDEX_WHEREEVER, FLAG_NOLIMIT);
							startDecay(splash);
						}

						break;
					}

					case COMBAT_ENERGYDAMAGE:
					{
						textColor = TEXTCOLOR_LIGHTBLUE;
						hitEffect = NM_ME_ENERGY_DAMAGE;
						break;
					}

					case COMBAT_POISONDAMAGE:
					{
						textColor = TEXTCOLOR_LIGHTGREEN;
						hitEffect = NM_ME_POISON_RINGS;
						break;
					}

					case COMBAT_FIREDAMAGE:
					{
						textColor = TEXTCOLOR_ORANGE;
						hitEffect = NM_ME_HITBY_FIRE;
						break;
					}

					case COMBAT_LIFEDRAIN:
					{
						textColor = TEXTCOLOR_RED;
						hitEffect = NM_ME_MAGIC_BLOOD;
						break;
					}

					default:
						break;
				}

				if(customHitEffect != NM_ME_UNK)
					hitEffect = customHitEffect;

				if(customTextColor != TEXTCOLOR_UNK)
					textColor = customTextColor;

				if(textColor != TEXTCOLOR_NONE){
					std::stringstream ss;
					ss << damage;
					addMagicEffect(list, targetPos, hitEffect);
					addAnimatedText(list, targetPos, textColor, ss.str());
				}
			}
		}
	}

	return true;
}

bool Game::combatChangeMana(Creature* attacker, Creature* target, int32_t manaChange)
{
	const Position& targetPos = target->getPosition();

	const SpectatorVec& list = getSpectators(targetPos);

	if(manaChange > 0){
		target->changeMana(manaChange);
	}
	else{
		if(!target->isAttackable() || Combat::canDoCombat(attacker, target) != RET_NOERROR){
			addMagicEffect(list, targetPos, NM_ME_PUFF);
			return false;
		}

		int32_t manaLoss = std::min(target->getMana(), -manaChange);
		BlockType_t blockType = target->blockHit(attacker, COMBAT_MANADRAIN, manaLoss);

		if(blockType != BLOCK_NONE){
			addMagicEffect(list, targetPos, NM_ME_PUFF);
			return false;
		}

		if(manaLoss > 0){
			target->drainMana(attacker, manaLoss);

			std::stringstream ss;
			ss << manaLoss;
			addAnimatedText(list, targetPos, TEXTCOLOR_BLUE, ss.str());
		}
	}

	return true;
}

void Game::addCreatureHealth(const Creature* target)
{
	const SpectatorVec& list = getSpectators(target->getPosition());
	addCreatureHealth(list, target);
}

void Game::addCreatureHealth(const SpectatorVec& list, const Creature* target)
{
	Player* player = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((player = (*it)->getPlayer())){
			player->sendCreatureHealth(target);
		}
	}
}

void Game::addAnimatedText(const Position& pos, uint8_t textColor,
	const std::string& text)
{
	const SpectatorVec& list = getSpectators(pos);

	addAnimatedText(list, pos, textColor, text);
}

void Game::addAnimatedText(const SpectatorVec& list, const Position& pos, uint8_t textColor,
	const std::string& text)
{
	Player* player = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((player = (*it)->getPlayer())){
			player->sendAnimatedText(pos, textColor, text);
		}
	}
}

void Game::addMagicEffect(const Position& pos, uint8_t effect)
{
	const SpectatorVec& list = getSpectators(pos);

	addMagicEffect(list, pos, effect);
}

void Game::addMagicEffect(const SpectatorVec& list, const Position& pos, uint8_t effect)
{
	Player* player = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((player = (*it)->getPlayer())){
			player->sendMagicEffect(pos, effect);
		}
	}
}

void Game::addDistanceEffect(const Position& fromPos, const Position& toPos,
	uint8_t effect)
{
	SpectatorVec list;
	getSpectators(list, fromPos, false);
	getSpectators(list, toPos, true);

	//send to client
	Player* tmpPlayer = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendDistanceShoot(fromPos, toPos, effect);
		}
	}
}

#ifdef __SKULLSYSTEM__
void Game::updateCreatureSkull(Player* player)
{
	const SpectatorVec& list = getSpectators(player->getPosition());

	//send to client
	Player* tmpPlayer = NULL;
	for(SpectatorVec::const_iterator it = list.begin(); it != list.end(); ++it){
		if((tmpPlayer = (*it)->getPlayer())){
			tmpPlayer->sendCreatureSkull(player);
		}
	}
}
#endif


void Game::startDecay(Item* item)
{
	if(item && item->canDecay()){
		uint32_t decayState = item->getDecaying();
		if(decayState == DECAYING_TRUE){
			//already decaying
			return;
		}

		int32_t dur = item->getDuration();
		if(dur > 0){
			item->useThing2();
			item->setDecaying(DECAYING_TRUE);
			toDecayItems.push_back(item);
		}
		else{
			internalDecayItem(item);
		}
	}
}

void Game::internalDecayItem(Item* item)
{
	const ItemType& it = Item::items[item->getID()];

	if(it.decayTo != 0){
		Item* newItem = transformItem(item, it.decayTo);
		startDecay(newItem);
	}
	else{
		ReturnValue ret = internalRemoveItem(item);

		if(ret != RET_NOERROR){
			std::cout << "DEBUG, internalDecayItem failed, error code: " << (int) ret << "item id: " << item->getID() << std::endl;
		}
	}
}

void Game::checkDecay()
{
	Scheduler::getScheduler().addEvent(createSchedulerTask(EVENT_DECAYINTERVAL,
		boost::bind(&Game::checkDecay, this)));
	
	size_t bucket = (last_bucket + 1) % EVENT_DECAY_BUCKETS;

	for(DecayList::iterator it = decayItems[bucket].begin(); it != decayItems[bucket].end();){
		Item* item = *it;
		int32_t decreaseTime = EVENT_DECAYINTERVAL*EVENT_DECAY_BUCKETS;
		if(item->getDuration() - decreaseTime < 0){
			decreaseTime = item->getDuration();
		}
		item->decreaseDuration(decreaseTime);
		//std::cout << "checkDecay: " << item << ", id:" << item->getID() << ", name: " << item->getName() << ", duration: " << item->getDuration() << std::endl;

		if(!item->canDecay()){
			item->setDecaying(DECAYING_FALSE);
			FreeThing(item);
			it = decayItems[bucket].erase(it);
			continue;
		}

		int32_t dur = item->getDuration();

		if(dur <= 0) {
			it = decayItems[bucket].erase(it);
			internalDecayItem(item);
			FreeThing(item);
		}
		else if(dur < EVENT_DECAYINTERVAL*EVENT_DECAY_BUCKETS)
		{
			it = decayItems[bucket].erase(it);
			size_t new_bucket = (bucket + ((dur + EVENT_DECAYINTERVAL/2) / 1000)) % EVENT_DECAY_BUCKETS;
			if(new_bucket == bucket) {
				internalDecayItem(item);
				FreeThing(item);
			} else {
				decayItems[new_bucket].push_back(item);
			}
		}
		else{
			++it;
		}
	}

	last_bucket = bucket;

	cleanup();
}

void Game::checkLight()
{
	Scheduler::getScheduler().addEvent(createSchedulerTask(EVENT_LIGHTINTERVAL,
		boost::bind(&Game::checkLight, this)));

	light_hour = light_hour + light_hour_delta;
	if(light_hour > 1440)
		light_hour = light_hour - 1440;

	if(std::abs(light_hour - SUNRISE) < 2*light_hour_delta){
		light_state = LIGHT_STATE_SUNRISE;
	}
	else if(std::abs(light_hour - SUNSET) < 2*light_hour_delta){
		light_state = LIGHT_STATE_SUNSET;
	}

	int newlightlevel = lightlevel;
	bool lightChange = false;
	switch(light_state){
	case LIGHT_STATE_SUNRISE:
		newlightlevel += (LIGHT_LEVEL_DAY - LIGHT_LEVEL_NIGHT)/30;
		lightChange = true;
		break;
	case LIGHT_STATE_SUNSET:
		newlightlevel -= (LIGHT_LEVEL_DAY - LIGHT_LEVEL_NIGHT)/30;
		lightChange = true;
		break;
	default:
		break;
	}

	if(newlightlevel <= LIGHT_LEVEL_NIGHT){
		lightlevel = LIGHT_LEVEL_NIGHT;
		light_state = LIGHT_STATE_NIGHT;
	}
	else if(newlightlevel >= LIGHT_LEVEL_DAY){
		lightlevel = LIGHT_LEVEL_DAY;
		light_state = LIGHT_STATE_DAY;
	}
	else{
		lightlevel = newlightlevel;
	}

	if(lightChange){
		LightInfo lightInfo;
		getWorldLightInfo(lightInfo);
		for(AutoList<Player>::listiterator it = Player::listPlayer.list.begin(); it != Player::listPlayer.list.end(); ++it){
			(*it).second->sendWorldLight(lightInfo);
		}
	}
}

void Game::getWorldLightInfo(LightInfo& lightInfo)
{
	lightInfo.level = lightlevel;
	lightInfo.color = 0xD7;
}

bool Game::cancelRuleViolation(Player* player)
{
	RuleViolationsMap::iterator it = ruleViolations.find(player->getID());
	if(it == ruleViolations.end()){
		return false;
	}

	Player* gamemaster = it->second->gamemaster;
	if(!it->second->isOpen && gamemaster){
		//Send to the responder
		gamemaster->sendRuleViolationCancel(player->getName());
	}
	else{
		//Send to channel
		ChatChannel* channel = g_chat.getChannelById(CHANNEL_RVR);
		if(channel){
			for(UsersMap::const_iterator ut = channel->getUsers().begin(); ut != channel->getUsers().end(); ++ut){
				if(ut->second){
					ut->second->sendRemoveReport(player->getName());
				}
			}
		}
	}

	//Now erase it
	ruleViolations.erase(it);
	return true;
}

bool Game::closeRuleViolation(Player* player)
{
	RuleViolationsMap::iterator it = ruleViolations.find(player->getID());
	if(it == ruleViolations.end()){
		return false;
	}

	ruleViolations.erase(it);
	player->sendLockRuleViolation();

	ChatChannel* channel = g_chat.getChannelById(CHANNEL_RVR);
	if(channel){
		for(UsersMap::const_iterator ut = channel->getUsers().begin(); ut != channel->getUsers().end(); ++ut){
			if(ut->second){
				ut->second->sendRemoveReport(player->getName());
			}
		}
	}

	return true;
}

void Game::addCommandTag(std::string tag)
{
	bool found = false;
	for(uint32_t i=0; i< commandTags.size() ;i++){
		if(commandTags[i] == tag){
			found = true;
			break;
		}
	}

	if(!found){
		commandTags.push_back(tag);
	}
}

void Game::resetCommandTag()
{
	commandTags.clear();
}

void Game::shutdown()
{
	std::cout << "Shutting down server...";
	
	Scheduler::getScheduler().shutdown();
	Dispatcher::getDispatcher().shutdown();
	Spawns::getInstance()->clear();
	g_bans.clearTemporaryBans();

	cleanup();

	if(g_server){
		g_server->stop();
	}
	std::cout << "[done]" << std::endl;
}

void Game::cleanup()
{
	//free memory
	for(std::vector<Thing*>::iterator it = ToReleaseThings.begin(); it != ToReleaseThings.end(); ++it){
		(*it)->releaseThing2();
	}

	ToReleaseThings.clear();

	for(DecayList::iterator it = toDecayItems.begin(); it != toDecayItems.end(); ++it){
		int32_t dur = (*it)->getDuration();
		if(dur >= EVENT_DECAYINTERVAL * EVENT_DECAY_BUCKETS) {
			decayItems[last_bucket].push_back(*it);
		} else {
			decayItems[(last_bucket + 1 + (*it)->getDuration() / 1000) % EVENT_DECAY_BUCKETS].push_back(*it);
		}
	}

	toDecayItems.clear();
}

void Game::FreeThing(Thing* thing)
{
	//std::cout << "freeThing() " << thing <<std::endl;
	ToReleaseThings.push_back(thing);
}


void Game::checkPlayersRecord()
{
	if(getPlayersOnline() > lastPlayersRecord){
		Database* db = Database::instance();
		DBQuery query;

		lastPlayersRecord = getPlayersOnline();
		query << "UPDATE `server_record` SET `record` = " << lastPlayersRecord << ";";
		db->executeQuery(query.str());
		query.str("");

		char buffer[50];
		sprintf(buffer, "New record of players online is: %d", lastPlayersRecord);
		AutoList<Player>::listiterator it = Player::listPlayer.list.begin();
        while(it != Player::listPlayer.list.end()){
            (*it).second->sendTextMessage(MSG_EVENT_ADVANCE, buffer);
            ++it;
        }
	}
}

void Game::loadPlayersRecord()
{
	Database* db = Database::instance();
	DBResult* result;

	if(!(result = db->storeQuery("SELECT `record` FROM `server_record`"))){
		std::cout << "> ERROR: Failed to load online record!" << std::endl;
		return;
	}

	lastPlayersRecord = result->getDataInt("record");
	db->freeResult(result);
}


void Game::doDeathDelay(Creature* creature)
{
    if (!creature || creature->isRemoved())
        return;
               
    creature->onDie();
    removeCreature(creature, false);
}

bool Game::bugReport(uint32_t playerId, std::string report)
{
   Player* player = getPlayerByID(playerId);
    if(!player || player->isRemoved())
        return false;

    if(report == " " || report == ""){
        player->sendTextMessage(MSG_STATUS_CONSOLE_BLUE, "Please enter a text you want to send.");
        return false;
    }
    
    std::string filename;
    filename = g_config.getString(ConfigManager::DATA_DIRECTORY) + "reports/" + player->getName() + ".txt";
    std::ofstream reportFile(filename.c_str(),std::ios::app);
    reportFile << "------------------------------------------------------\n";
    reportFile << "Name: " << player->getName() << " - Level: " << player->getLevel() << " - Access: " << player->getAccessLevel() << "\n";
    reportFile << "Position: " << player->getPosition().x << " " << player->getPosition().y << " " << player->getPosition().z << " - Skull: " << player->getSkull() << "\n";
    reportFile << "Report: " << report << "\n";
    reportFile << "------------------------------------------------------\n";
    reportFile.close();
    
    player->sendTextMessage(MSG_STATUS_CONSOLE_RED, "Your report has been sent.");
    return true;
}

bool Game::violationWindow(uint32_t playerId, std::string name, uint8_t reason, std::string comment, uint8_t action, bool IPBanishment)
{
	Player* player = getPlayerByID(playerId);
	if(!player || player->isRemoved()) {
		return false;
	}

	bool playerExists = IOPlayer::instance()->playerExists(name);
	if (!playerExists) {
		player->sendCancel("A player with this name does not exist.");
		return false;
	}

	Account account;
	uint32_t guid, ip;
	Player* targetPlayer = getPlayerByName(name);
	if (targetPlayer) {
		if (targetPlayer->hasFlag(PlayerFlag_CannotBeBanned)) {
			player->sendCancel("You do not have authorization for this action.");
			return false;
		}

		account = IOAccount::instance()->loadAccount(targetPlayer->getAccount());
		guid = targetPlayer->getGUID();
		ip = targetPlayer->lastip;
	}
	else {
		if (IOPlayer::instance()->hasFlag(name, PlayerFlag_CannotBeBanned)) {
			player->sendCancel("You do not have authorization for this action.");
			return false;
		}

		account = IOAccount::instance()->loadAccount(IOPlayer::instance()->getAccountIdByName(name));
		ip = IOPlayer::instance()->getLastIP(name);
	}

	if (IOPlayer::instance()->getAccessByName(name) >= player->getAccessLevel()) {
		player->sendCancel("You do not have authorization for this action.");
		return false;
	}

	std::string banReason;
				
	switch(reason)
	{
		case 0:
			banReason = "offensive name";
			break;
		case 1:
			banReason = "name containing part of sentence";
			break;
		case 2:
			banReason = "name with nonsensical letter combo";
			break;
		case 3:
			banReason = "invalid name format";
			break;
		case 4:
			banReason = "name not describing person";
			break;
		case 5:
			banReason = "name of celebrity";
			break;
		case 6:
			banReason = "name referring to country";
			break;
		case 7:
			banReason = "namefaking player identity";
			break;
		case 8:
			banReason = "namefaking official position";
		    break;
		case 9:
			banReason = "offensive statement";
			break;
		case 10:
			banReason = "spamming";
			break;
		case 11:
			banReason = "advertisement not related to game";
			break;
		case 12:
			banReason = "real money advertisement";
			break;
		case 13:
			banReason = "Non-English public statement";
			break;
		case 14:
			banReason = "off-topic public statement";
			break;
		case 15:
			banReason = "inciting rule violation";
			break;
	    case 16:
			banReason = "bug abuse";
			break;
		case 17:
			banReason = "game weakness abuse";
			break;
		case 18:
			banReason = "using macro";
			break;
		case 19:
			banReason = "using unofficial software to play";
			break;
		case 20:
			banReason = "hacking";
			break;
		case 21:
			banReason = "multi-clienting";
			break;
		case 22:
			banReason = "account trading";
			break;
		case 23:
			banReason = "account sharing";
			break;
		case 24:
			banReason = "threatening gamemaster";
			break;
		case 25:
			banReason = "pretending to have official position";
			break;
		case 26:
			banReason = "pretending to have influence on gamemaster";
			break;
		case 27:
			banReason = "false report to gamemaster";
			break;
		case 28:
			banReason = "excessive unjustifed player killing";
			break;
		case 29:
			banReason = "destructive behaviour";
			break;
		case 30:
			banReason = "spoiling auction";
			break;
		case 31:
			banReason = "invalid payment";
			break;
		default:
			banReason = "nothing";
			break;
	}

	bool notation = false;
	switch(action)
	{
		// Notation
	    case 0: 
		{
			g_bans.addNotation(name, player->getGUID(), banReason, comment);
			if (g_bans.getNotationsCount(account.accnumber >= (uint32_t)g_config.getNumber(ConfigManager::NOTATIONS_TO_BAN))) {
				account.warnings++;
				if (account.warnings >= (g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION))) {
					g_bans.addDeletion(name, player->getGUID(), banReason, comment);
				}
				else if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_FINALBAN)) {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
				}
				else {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::BAN_LENGTH)), player->getGUID(), banReason, comment);
				}
			}
			else {
				notation = true;
			}
			break;
		}
		// Namelock
		case 1:
		{
			g_bans.addNamelock(name, player->getGUID(), banReason, comment);
			break;
		}
		// AccountBan
		case 2:
		{
			if (account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION)) {
				g_bans.addDeletion(name, player->getGUID(), banReason, comment);
			}
			else {
				account.warnings++;
				if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_FINALBAN)) {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
				}
				else {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::BAN_LENGTH)), player->getGUID(), banReason, comment);
				}
			}
			break;
		}
		// Namelock/AccountBan
		case 3:
		{
			if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION)) {
				g_bans.addDeletion(name, player->getGUID(), banReason, comment);
			}
			else {
				account.warnings++;
				if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_FINALBAN)) {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
				}
				else {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::BAN_LENGTH)), player->getGUID(), banReason, comment);
				}

				g_bans.addNamelock(name, player->getGUID(), banReason, comment);
			}
			break;
		}
		// AccountBan + Final Warning
		case 4:
		{
			if(account.warnings < g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION)) {
				account.warnings = g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION);
				g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
			}
			else {
				g_bans.addDeletion(name, player->getGUID(), banReason, comment);
			}
			break;
		}
		// Namelock/AccountBan + Final Warning
		case 5:
		{
			if(account.warnings < g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION)) {
				account.warnings = g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION);
				g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
				g_bans.addNamelock(name, player->getGUID(), banReason, comment);
			}
			else {
				g_bans.addDeletion(name, player->getGUID(), banReason, comment);
			}
			break;
		}
		default:
		{
			account.warnings++;
			if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_DELETION)) {
				g_bans.addDeletion(name, player->getGUID(), banReason, comment);
			}
			else {
				if(account.warnings >= g_config.getNumber(ConfigManager::WARNINGS_TO_FINALBAN)) {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::FINALBAN_LENGTH)), player->getGUID(), banReason, comment);
				}
				else {
					g_bans.addBanishment(name, (std::time(NULL) + g_config.getNumber(ConfigManager::BAN_LENGTH)), player->getGUID(), banReason, comment);
				}
			}
			break;
		}
	}

	if(IPBanishment && ip > 0) {
		g_bans.addIpBanishment(ip, (std::time(NULL) + g_config.getNumber(ConfigManager::IPBANISHMENT_LENGTH)), player->getGUID(), banReason, comment);
	}

	if(targetPlayer && !notation){
		addMagicEffect(targetPlayer->getPosition(), NM_ME_MAGIC_POISON);

		uint32_t playerId = targetPlayer->getID();
		Scheduler::getScheduler().addEvent(createSchedulerTask(1000, boost::bind(&Player::kickPlayer, targetPlayer)));
	}

	IOAccount::instance()->saveAccount(account);
	return true;
}

uint64_t Game::getExperienceStage(uint32_t level)
{
	if(stagesEnabled){
		if(useLastStageLevel && level >= lastStageLevel){
			return stages[lastStageLevel];
		}
		else{
			return stages[level];
		}
	}
	else{
		return g_config.getNumber(ConfigManager::RATE_EXPERIENCE);
	}
}

bool Game::loadExperienceStages()
{
	std::string filename = "data/stages.xml";
	xmlDocPtr doc = xmlParseFile(filename.c_str());
	if(doc){
		xmlNodePtr root, p;
		int32_t intVal, low, high, mult;
		root = xmlDocGetRootElement(doc);
		if(xmlStrcmp(root->name,(const xmlChar*)"stages")){
			xmlFreeDoc(doc);
			return false;
		}
		
		p = root->children;
		while(p){
			if(!xmlStrcmp(p->name, (const xmlChar*)"config")){
				if(readXMLInteger(p, "enabled", intVal)){
					stagesEnabled = (intVal != 0);
				}
			}
			else if(!xmlStrcmp(p->name, (const xmlChar*)"stage")){
				if(readXMLInteger(p, "minlevel", intVal)){
					low = intVal;
				}	
				else{
					low = 1;
				}

				if(readXMLInteger(p, "maxlevel", intVal)){
					high = intVal;
				}
				else{
					high = 0;
					lastStageLevel = low;
					useLastStageLevel = true;
				}

				if(readXMLInteger(p, "multiplier", intVal)){
					mult = intVal;
				}
				else{
					mult = 1;
				}

				if(useLastStageLevel){
					stages[lastStageLevel] = mult;
				}
				else{
					for(int32_t iteratorValue = low; iteratorValue <= high; iteratorValue++){
						stages[iteratorValue] = mult;
					}
				}
			}
			p = p->next;
		}
		xmlFreeDoc(doc);
	}
	return true;
}
