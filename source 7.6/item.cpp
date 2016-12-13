//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// Represents an item
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
#include "item.h"
#include "container.h"
#include "depot.h"
#include "teleport.h"
#include "trashholder.h"
#include "mailbox.h"
#include "house.h"
#include "game.h"
#include "luascript.h"
#include "configmanager.h"

#include "actions.h"
#include "combat.h"
#include "weapons.h"
//[ added for beds system
#include "beds.h"
//]

#include <iostream>
#include <sstream>
#include <iomanip>

extern Game g_game;
extern Weapons* g_weapons;
extern ConfigManager g_config;

Items Item::items;

Item* Item::CreateItem(const uint16_t _type, uint16_t _count /*= 1*/)
{
	Item* newItem = NULL;

	const ItemType& it = Item::items[_type];

	if(it.id != 0){
		if(it.isDepot()){
			newItem = new Depot(_type);
		}
		else if(it.isContainer()){
			newItem = new Container(_type);
		}
		else if(it.isTeleport()){
			newItem = new Teleport(_type);
		}
		else if(it.isMagicField()){
			newItem = new MagicField(_type);
		}
		else if(it.isDoor()){
			newItem = new Door(_type);
		}
		else if(it.isTrashHolder()){
			newItem = new TrashHolder(_type, it.magicEffect);
		}
		else if(it.isMailbox()){
			newItem = new Mailbox(_type);
		}
		//[ added for beds system
		else if(it.isBed()){
			newItem = new BedItem(_type);
		}
		//]
		else{
			newItem = new Item(_type, _count);
		}

		newItem->useThing2();
	}
	return newItem;
}

Item* Item::CreateItem(PropStream& propStream)
{
	uint16_t _id;
	if(!propStream.GET_USHORT(_id)){
		return NULL;
	}

	const ItemType& iType = Item::items[_id];
	uint8_t _count = 0;

	if(iType.stackable || iType.isSplash() || iType.isFluidContainer()){
		if(!propStream.GET_UCHAR(_count)){
			return NULL;
		}
	}

	return Item::CreateItem(_id, _count);
}

Item::Item(const uint16_t _type, uint16_t _count /*= 0*/) :
	ItemAttributes()
{
	id = _type;

	const ItemType& it = items[id];

	count = 1;
	if(it.charges != 0){
		setCharges(it.charges);
	}
	
	if(it.isFluidContainer() || it.isSplash()){
		setFluidType(_count);
	}
	else if(it.stackable && _count != 0){
		count = _count;
	}
	else if(it.charges != 0 && _count != 0){
		setCharges(_count);
	}

	setDefaultDuration();
	
	loadedOnMap = false;
}

Item::Item(const Item &i) :
	Thing(), ItemAttributes()
{
	//std::cout << "Item copy constructor " << this << std::endl;
	id = i.id;
	count = i.count;

	m_attributes = i.m_attributes;
	if(i.m_firstAttr){
		m_firstAttr = new Attribute(*i.m_firstAttr);
	}
}

Item* Item::clone() const
{
	Item* _item = Item::CreateItem(id, count);
	_item->m_attributes = m_attributes;
	if(m_firstAttr){
		_item->m_firstAttr = new Attribute(*m_firstAttr);
	}

	return _item;
}

void Item::copyAttributes(Item* item)
{
	m_attributes = item->m_attributes;
	if(item->m_firstAttr){
		m_firstAttr = new Attribute(*item->m_firstAttr);
	}

	removeAttribute(ATTR_ITEM_DECAYING);
	removeAttribute(ATTR_ITEM_DURATION);
}

Item::~Item()
{
	//std::cout << "Item destructor " << this << std::endl;

	if(getUniqueId() != 0){
		ScriptEnviroment::removeUniqueThing(this);
	}
}

void Item::setDefaultSubtype()
{
	const ItemType& it = items[id];

	count = 1;
	if(it.charges != 0){
		setCharges(it.charges);
	}
}

void Item::setID(uint16_t newid)
{
	const ItemType& prevIt = Item::items[id];
	id = newid;

	const ItemType& it = Item::items[newid];
	uint32_t newDuration = it.decayTime * 1000;

	if(newDuration == 0 && !it.stopTime && it.decayTo == -1){
		removeAttribute(ATTR_ITEM_DECAYING);
		removeAttribute(ATTR_ITEM_DURATION);
	}

	if(newDuration > 0 && (!prevIt.stopTime || !hasAttribute(ATTR_ITEM_DURATION)) ){
		setDecaying(DECAYING_FALSE);
		setDuration(newDuration);
	}
}

bool Item::hasSubType() const
{
	const ItemType& it = items[id];
	return it.hasSubType();
}

uint16_t Item::getSubType() const
{
	const ItemType& it = items[getID()];

	if(it.isFluidContainer() || it.isSplash()){
		return getFluidType();
	}
	else if(it.charges != 0){
		return getCharges();
	}

	return count;
}

void Item::setSubType(uint16_t n)
{
	const ItemType& it = items[id];

	if(it.isFluidContainer() || it.isSplash()){
		setFluidType(n);
	}
	else if(it.charges != 0){
		setCharges(n);
	}
	else{
		count = n;
	}
}

bool Item::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	switch(attr){
		case ATTR_COUNT:
		{
			uint8_t _count = 0;
			if(!propStream.GET_UCHAR(_count)){
				return false;
			}

			setSubType(_count);
			break;
		}

		case ATTR_ACTION_ID:
		{
			uint16_t _actionid = 0;
			if(!propStream.GET_USHORT(_actionid)){
				return false;
			}

			setActionId(_actionid);
			break;
		}

		case ATTR_UNIQUE_ID:
		{
			uint16_t _uniqueid;
			if(!propStream.GET_USHORT(_uniqueid)){
				return false;
			}

			setUniqueId(_uniqueid);
			break;
		}

		case ATTR_TEXT:
		{
			std::string _text;
			if(!propStream.GET_STRING(_text)){
				return false;
			}

			setText(_text);
			break;
		}

		case ATTR_WRITTENDATE:
		{
			uint32_t _writtenDate;
			if(!propStream.GET_ULONG(_writtenDate)){
				return false;
			}

			setWrittenDate(_writtenDate);
			break;
		}

		case ATTR_WRITTENBY:
		{
			std::string _writer;
			if(!propStream.GET_STRING(_writer)){
				return false;
			}

			setWriter(_writer);
			break;
		}

		case ATTR_DESC:
		{
			std::string _text;
			if(!propStream.GET_STRING(_text)){
				return false;
			}

			setSpecialDescription(_text);
			break;
		}

		case ATTR_RUNE_CHARGES:
		{
			uint8_t _charges = 1;
			if(!propStream.GET_UCHAR(_charges)){
				return false;
			}

			setSubType(_charges);
			break;
		}

		case ATTR_DURATION:
		{
			uint32_t duration = 0;
			if(!propStream.GET_ULONG(duration)){
				return false;
			}

			if(((int32_t)duration) < 0){
				duration = 0;
			}
			setDuration(duration);
			break;
		}

		case ATTR_DECAYING_STATE:
		{
			uint8_t state = 0;
			if(!propStream.GET_UCHAR(state)){
				return false;
			}

			if(state != DECAYING_FALSE){
				setDecaying(DECAYING_PENDING);
			}
			break;
		}

		//these should be handled through derived classes
		//If these are called then something has changed in the items.otb since the map was saved
		//just read the values

		//Depot class
		case ATTR_DEPOT_ID:
		{
			uint16_t _depotId;
			if(!propStream.GET_USHORT(_depotId)){
				return false;
			}

			return true;
		}

		//Door class
		case ATTR_HOUSEDOORID:
		{
			uint8_t _doorId;
			if(!propStream.GET_UCHAR(_doorId)){
				return false;
			}

			return true;
		}

		//Teleport class
		case ATTR_TELE_DEST:
		{
			TeleportDest* tele_dest;
			if(!propStream.GET_STRUCT(tele_dest)){
				return false;
			}

			return true;
		}

		default:
			return false;
		break;
	}

	return true;
}

bool Item::unserializeAttr(PropStream& propStream)
{
	uint8_t attr_type;
	while(propStream.GET_UCHAR(attr_type) && attr_type != 0){
		if(!readAttr((AttrTypes_t)attr_type, propStream)){
			return false;
			break;
		}
	}

	return true;
}

bool Item::unserializeItemNode(FileLoader& f, NODE node, PropStream& propStream)
{
	return unserializeAttr(propStream);
}

bool Item::serializeAttr(PropWriteStream& propWriteStream) const
{
	if(isStackable() || isSplash() || isFluidContainer()){
		uint8_t _count = getSubType();
		propWriteStream.ADD_UCHAR(ATTR_COUNT);
		propWriteStream.ADD_UCHAR(_count);
	}

	if(hasCharges()){
		uint8_t _count = getCharges();
		propWriteStream.ADD_UCHAR(ATTR_RUNE_CHARGES);
		propWriteStream.ADD_UCHAR(_count);
	}

	if(!isNotMoveable() /*moveable*/){
		uint16_t _actionId = getActionId();
		if(_actionId){
			propWriteStream.ADD_UCHAR(ATTR_ACTION_ID);
			propWriteStream.ADD_USHORT(_actionId);
		}
	}

	const std::string& _text = getText();
	if(_text.length() > 0){
		propWriteStream.ADD_UCHAR(ATTR_TEXT);
		propWriteStream.ADD_STRING(_text);
	}

	const time_t _writtenDate = getWrittenDate();
	if(_writtenDate > 0){
		propWriteStream.ADD_UCHAR(ATTR_WRITTENDATE);
		propWriteStream.ADD_ULONG(_writtenDate);
	}

	const std::string& _writer = getWriter();
	if(_writer.length() > 0){
		propWriteStream.ADD_UCHAR(ATTR_WRITTENBY);
		propWriteStream.ADD_STRING(_writer);
	}

	const std::string& _specialDesc = getSpecialDescription();
	if(_specialDesc.length() > 0){
		propWriteStream.ADD_UCHAR(ATTR_DESC);
		propWriteStream.ADD_STRING(_specialDesc);
	}

	uint32_t duration = getDuration();
	if(duration != 0){
		propWriteStream.ADD_UCHAR(ATTR_DURATION);
		propWriteStream.ADD_ULONG(duration);
	}

	uint32_t decayState = getDecaying();
	if(decayState == DECAYING_TRUE || decayState == DECAYING_PENDING){
		propWriteStream.ADD_UCHAR(ATTR_DECAYING_STATE);
		propWriteStream.ADD_UCHAR(decayState);
	}

	return true;
}

bool Item::hasProperty(enum ITEMPROPERTY prop) const
{
	const ItemType& it = items[id];

	switch(prop){
		case BLOCKSOLID:
			if(it.blockSolid)
				return true;
			break;

		case MOVEABLE:
			if(it.moveable && getUniqueId() == 0)
				return true;
			break;

		case HASHEIGHT:
			if(it.hasHeight)
				return true;
			break;

		case BLOCKPROJECTILE:
			if(it.blockProjectile)
				return true;
			break;

		case BLOCKPATH:
			if(it.blockPathFind)
				return true;
			break;

		case ISVERTICAL:
			if(it.isVertical)
				return true;
			break;

		case ISHORIZONTAL:
			if(it.isHorizontal)
				return true;
			break;

		case IMMOVABLEBLOCKSOLID:
			if(it.blockSolid && (!it.moveable || getUniqueId() != 0))
				return true;
			break;
			
		case IMMOVABLEBLOCKPATH:
            if(it.blockPathFind && (!it.moveable || getUniqueId() != 0))
                return true;
            break;
		
		case SUPPORTHANGABLE:
            if(it.isHorizontal || it.isVertical)
                return true;
            break;
            
        case IMMOVABLENOFIELDBLOCKPATH:
            if(!it.isMagicField() && it.blockPathFind && (!it.moveable || getUniqueId() != 0))
                return true;
            break;
            
        case NOFIELDBLOCKPATH:
            if(!it.isMagicField() && it.blockPathFind)
                return true;
            break;

		default:
			return false;
	}
	return false;
}

double Item::getWeight() const
{
	if(isStackable()){
		return items[id].weight * std::max((int32_t)1, (int32_t)count);
	}

	return items[id].weight;
}

std::string Item::getDescription(const ItemType& it, int32_t lookDistance,
	const Item* item /*= NULL*/, int32_t subType /*= -1*/)
{
	std::stringstream s;

	if(item){
		subType = item->getSubType();
	}

	if(it.name.length()){
		if(it.stackable && item && item->getItemCount() > 1){
			if(it.showCount){
				s << (int)item->getItemCount() << " ";
			}

			s << it.pluralName;
		}
		else{
			if(!it.article.empty()){
				s << it.article << " ";
			}
			s << it.name;
		}
	}
	else{
		s << "an item of type " << it.id;
	}

	if(it.isRune()){
		uint32_t charges = std::max((uint32_t)1, (uint32_t)(item == NULL ? it.charges : item->getCharges()));

		if(it.runeLevel > 0 || it.runeMagLevel > 0){
			if(it.runeLevel > 0){
				s << " for level " << it.runeLevel;
			}
			if(it.runeMagLevel > 0){
				if(it.runeLevel > 0){
					s << " and";
				}
				s << " for magic level " << it.runeMagLevel;
			}
		}
		s << ". It's " << it.article << " \"" << it.runeSpellName << "\"-spell (" << charges << "x). ";
	}
	else if(it.weaponType != WEAPON_NONE){
		if(it.weaponType == WEAPON_DIST && it.amuType != AMMO_NONE){
			s << " (Range:" << it.shootRange;
			if(it.attack != 0){
				s << ", Atk" << std::showpos << it.attack << std::noshowpos;
			}
			if(it.hitChance != 0){
				s << ", Hit%" << std::showpos << it.hitChance << std::noshowpos;
			}
			s << ")";
		}
		else if(it.weaponType != WEAPON_AMMO && it.weaponType != WEAPON_WAND){ // Arrows and Bolts doesn't show atk
			s << " (";
			if(it.attack != 0){
				s << "Atk:" << (int)it.attack;
			}

			if(it.defence != 0 || it.extraDef != 0){
				if(it.attack != 0)
					s << " ";

				s << "Def:" << (int)it.defence;
				if(it.extraDef != 0){
					s << " " << std::showpos << (int)it.extraDef << std::noshowpos;
				}
			}

			if(it.abilities.stats[STAT_MAGICPOINTS] != 0){
				if(it.attack != 0 || it.defence != 0 || it.extraDef != 0)
					s << ", ";

				s << "magic level " << std::showpos << (int)it.abilities.stats[STAT_MAGICPOINTS] << std::noshowpos;
			}
			s << ")";
		}
		s << ".";
	}
	else if(it.armor != 0 || it.abilities.absorb.any()){
		if(it.showCharges){
			if(subType > 1){
				s << " that has " << (int32_t)subType << " charges left";
			}
			else{
				s << " that has 1 charge left";
			}
		}

		s << " (Arm:" << it.armor;

		if(it.abilities.absorb.any()){
			s << ", protection";
			it.abilities.absorb.getDescription(s);
		}
		s << ").";
	}
	else if(it.isFluidContainer()){
		if(item && item->getFluidType() != 0){
			s << " of " << items[item->getFluidType()].name << ".";
		}
		else{
			s << ". It is empty.";
		}
	}
	else if(it.isSplash()){
		s << " of ";
		if(item && item->getFluidType() != 0){
			s << items[item->getFluidType()].name;
		}
		else{
			s << items[1].name;
		}
	}
	else if(it.isContainer()){
		s << " (Vol:" << (int)it.maxItems << ").";
	}
	else if(it.isKey()){
		if(item){
			s << " (Key:" << (int)item->getActionId() << ").";
		}
		else{
			s << " (Key:" << (int)0 << ").";
		}
	}
	else if(it.allowDistRead){
		s << std::endl;

		if(item && item->getText() != ""){
			if(lookDistance <= 4){
				if(item->getWriter().length()){
					s << item->getWriter() << " wrote";

					time_t wDate = item->getWrittenDate();
					if(wDate > 0){
						char date[16];
						formatDate2(wDate, date);
						s << " on " << date;
					}
					s << ": ";
				}
				else{
					s << "You read: ";
				}

				s << item->getText();
			}
			else{
				s << "You are too far away to read it.";
			}
		}
		else{
			s << "Nothing is written on it.";
		}
	}
	else if(it.showCharges){
		uint32_t charges = (item == NULL ? it.charges : item->getCharges());
		if(charges > 1){
			s << " that has " << (int)charges << " charges left.";
		}
		else{
			s << " that has 1 charge left.";
		}
	}
	else if(it.showDuration){
		if(item && item->hasAttribute(ATTR_ITEM_DURATION)){
			int32_t duration = item->getDuration() / 1000;
			s << " that has energy for ";

			if(duration >= 120){
				s << duration / 60 << " minutes left.";
			}
			else if(duration > 60){
				s << "1 minute left.";
			}
			else{
				s << " less than a minute left.";
			}
		}
		else{
			s << " that is brand-new.";
		}
	}
	else{
		s << ".";
	}

	if(it.wieldInfo != 0){
		s << std::endl << "It can only be wielded properly by ";
		if(it.wieldInfo & WIELDINFO_PREMIUM){
			s << "premium ";
		}

		if(it.wieldInfo & WIELDINFO_VOCREQ){
			s << it.vocationString;
		}
		else{
			s << "players";
		}

		if(it.wieldInfo & WIELDINFO_LEVEL){
			s << " of level " << (int)it.minReqLevel << " or higher";
		}

		if(it.wieldInfo & WIELDINFO_MAGLV){
			if(it.wieldInfo & WIELDINFO_LEVEL){
				s << " and";
			}
			else{
				s << " of";
			}
			s << " magic level " << (int)it.minReqMagicLevel << " or higher";
		}

		s << ".";
	}

	if(lookDistance <= 1){
		double weight = (item == NULL ? it.weight : item->getWeight());
		if(weight > 0){
			s << std::endl << getWeightDescription(it, weight);
		}
	}

	if(item && item->getSpecialDescription() != ""){
		s << std::endl << item->getSpecialDescription().c_str();
	}
	else if(it.description.length() && lookDistance <= 1){
		s << std::endl << it.description;
	}

	return s.str();
}

std::string Item::getDescription(int32_t lookDistance) const
{
	std::stringstream s;
	const ItemType& it = items[id];

	s << getDescription(it, lookDistance, this);
	return s.str();
}

std::string Item::getWeightDescription() const
{
	double weight = getWeight();
	if(weight > 0){
		return getWeightDescription(weight);
	}
	else{
		return "";
	}
}

std::string Item::getWeightDescription(double weight) const
{
	const ItemType& it = Item::items[id];
	return getWeightDescription(it, weight, count);
}

std::string Item::getWeightDescription(const ItemType& it, double weight, uint32_t count /*= 1*/)
{
	std::stringstream ss;
	if(it.stackable && count > 1){
		ss << "They weigh " << std::fixed << std::setprecision(2) << weight << " oz.";
	}
	else{
		ss << "It weighs " << std::fixed << std::setprecision(2) << weight << " oz.";
	}
	return ss.str();
}

void Item::setUniqueId(uint16_t n)
{
	if(getUniqueId() != 0)
		return;

	ItemAttributes::setUniqueId(n);
	ScriptEnviroment::addUniqueThing(this);
}

bool Item::canDecay()
{
	if(isRemoved()){
		return false;
	}

	if(getUniqueId() != 0 || getActionId() != 0){
		return false;
	}

	return (items[id].decayTo != -1);
}

int Item::getWorth() const
{
	switch(getID()){
	case ITEM_COINS_GOLD:
		return getItemCount();
	case ITEM_COINS_PLATINUM:
		return getItemCount() * 100;
	case ITEM_COINS_CRYSTAL:
		return getItemCount() * 10000;
	default:
		return 0;
	}
}

void Item::getLight(LightInfo& lightInfo)
{
	const ItemType& it = items[id];
	lightInfo.color = it.lightColor;
	lightInfo.level = it.lightLevel;
}

std::string ItemAttributes::emptyString("");

const std::string& ItemAttributes::getStrAttr(itemAttrTypes type) const
{
	if(!validateStrAttrType(type))
		return emptyString;

	//this will *NOT* create the attribute if it does not exist
	Attribute* attr = getAttrConst(type);
	if(attr){
		return *(std::string*)attr->value;
	}
	else{
		return emptyString;
	}
}

void ItemAttributes::setStrAttr(itemAttrTypes type, const std::string& value)
{
	if(!validateStrAttrType(type))
		return;

	if(value.length() == 0)
		return;

	//this will create the attribute if it does not exist
	Attribute* attr = getAttr(type);
	if(attr){
		//if has a previous value delete it
		if(attr->value){
			delete (std::string*)attr->value;
		}
		//create the new value as string
		attr->value = (void*)new std::string(value);
	}
}

bool ItemAttributes::hasAttribute(itemAttrTypes type) const
{
	if(!validateIntAttrType(type))
		return false;

	Attribute* attr = getAttrConst(type);
	if(attr){
		return true;
	}

	return false;
}

void ItemAttributes::removeAttribute(itemAttrTypes type)
{
	//check if we have it
	if((type & m_attributes) != 0){
		//go trough the linked list until find it
		Attribute* prevAttr = NULL;
		Attribute* curAttr = m_firstAttr;
		while(curAttr != NULL){
			if(curAttr->type == type){
				//found so remove it from the linked list
				if(prevAttr){
					prevAttr->next = curAttr->next;
				}
				else{
					m_firstAttr = curAttr->next;
				}
				//remove it from flags
				m_attributes = m_attributes & ~type;

				//delete string if it is string type
				if(validateStrAttrType(type)){
					delete (std::string*)curAttr->value;
				}
				//finally delete the attribute and return
				delete curAttr;
				return;
			}

			//advance in the linked list
			prevAttr = curAttr;
			curAttr = curAttr->next;
		}
	}
}

uint32_t ItemAttributes::getIntAttr(itemAttrTypes type) const
{
	if(!validateIntAttrType(type))
		return 0;

	Attribute* attr = getAttrConst(type);
	if(attr){
		return static_cast<uint32_t>(0xFFFFFFFF & reinterpret_cast<ptrdiff_t>(attr->value));
	}
	else{
		return 0;
	}
}

void ItemAttributes::setIntAttr(itemAttrTypes type, int32_t value)
{
	if(!validateIntAttrType(type))
		return;

	Attribute* attr = getAttr(type);
	if(attr){
		attr->value = reinterpret_cast<void*>(static_cast<ptrdiff_t>(value));
	}
}

void ItemAttributes::increaseIntAttr(itemAttrTypes type, int32_t value)
{
	if(!validateIntAttrType(type))
		return;

	Attribute* attr = getAttr(type);
	if(attr){
		attr->value = reinterpret_cast<void*>(static_cast<ptrdiff_t>(static_cast<uint32_t>(0xFFFFFFFF & reinterpret_cast<ptrdiff_t>(attr->value)) + value));
	}
}

bool ItemAttributes::validateIntAttrType(itemAttrTypes type)
{
	//list of numeric type attributes
	switch(type){
	case ATTR_ITEM_ACTIONID:
	case ATTR_ITEM_UNIQUEID:
	case ATTR_ITEM_OWNER:
	case ATTR_ITEM_DURATION:
	case ATTR_ITEM_DECAYING:
	case ATTR_ITEM_WRITTENDATE:
    case ATTR_ITEM_CHARGES:
    case ATTR_ITEM_FLUIDTYPE:
    case ATTR_ITEM_DOORID:
		return true;
		break;

	default:
		return false;
		break;
	}
	return false;
}

bool ItemAttributes::validateStrAttrType(itemAttrTypes type)
{
	//list of text type attributes
	switch(type){
	case ATTR_ITEM_DESC:
	case ATTR_ITEM_TEXT:
	case ATTR_ITEM_WRITTENBY:
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

void ItemAttributes::addAttr(Attribute* attr)
{
	//add an attribute to the linked list
	if(m_firstAttr){
		//is not the first, so look for the end of the list
		Attribute* curAttr = m_firstAttr;
		while(curAttr->next){
			curAttr = curAttr->next;
		}
		//and add it at the end
		curAttr->next = attr;
	}
	else{
		//is the first
		m_firstAttr = attr;
	}
	//add it to flags
	m_attributes = m_attributes | attr->type;
}

ItemAttributes::Attribute* ItemAttributes::getAttrConst(itemAttrTypes type) const
{
	//check flags
	if((type & m_attributes) == 0){
		return NULL;
	}
	//it is here, so search it in the linked list
	Attribute* curAttr = m_firstAttr;
	while(curAttr){
		if(curAttr->type == type){
			//found
			return curAttr;
		}
		curAttr = curAttr->next;
	}
	//not found?
	std::cout << "Warning: [ItemAttributes::getAttrConst] (type & m_attributes) != 0 but attribute not found" << std::endl;
	return NULL;
}

ItemAttributes::Attribute* ItemAttributes::getAttr(itemAttrTypes type)
{
	Attribute* curAttr;
	if((type & m_attributes) == 0){
		//if that type was not present add it
		curAttr = new Attribute(type);
		addAttr(curAttr);
		return curAttr;
	}
	else{
		//was present, search and return it
		curAttr = m_firstAttr;
		while(curAttr){
			if(curAttr->type == type){
				return curAttr;
			}
			curAttr = curAttr->next;
		}
	}
	std::cout << "Warning: [ItemAttributes::getAttr] (type & m_attributes) != 0 but attribute not found" << std::endl;
	curAttr = new Attribute(type);
	addAttr(curAttr);
	return curAttr;
}

void ItemAttributes::deleteAttrs(Attribute* attr)
{
	//deletes all attributes recursively
	if(attr){
		//if is string type, delete the allocated string
		if(validateStrAttrType(attr->type)){
			delete (std::string*)attr->value;
		}
		Attribute* next_attr = attr->next;
		attr->next = NULL;
		//delete next while it was not NULL
		if(next_attr){
			deleteAttrs(next_attr);
		}
		delete attr;
	}
}

void Item::__startDecaying()
{
	g_game.startDecay(this);
}

bool Item::isCleanable() const
{
    if(loadedOnMap)
        return false;
    if(getUniqueId() != 0 || getActionId() != 0)
        return false;
    if(!isPickupable() && g_config.getBool(ConfigManager::CLEAN_CORPSES))
    {
        if(g_game.getGameState() != GAME_STATE_CLOSED || !g_config.getBool(ConfigManager::CLEAN_CORPSES_SAVE))
            return false;
    }
    if(!canRemove())
        return false;
         
    return true;
}
