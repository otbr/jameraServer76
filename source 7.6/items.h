//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
// The database of items.
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


#ifndef __OTSERV_ITEMS_H__
#define __OTSERV_ITEMS_H__


#include "definitions.h"
#include "const76.h"
#include "enums.h"
#include "itemloader.h"
//[ added for beds system
#include "position.h"
//]
#include <map>

#define SLOTP_WHEREEVER 0xFFFFFFFF
#define SLOTP_HEAD 1
#define	SLOTP_NECKLACE 2
#define	SLOTP_BACKPACK 4
#define	SLOTP_ARMOR 8
#define	SLOTP_RIGHT 16
#define	SLOTP_LEFT 32
#define	SLOTP_LEGS 64
#define	SLOTP_FEET 128
#define	SLOTP_RING 256
#define	SLOTP_AMMO 512
#define	SLOTP_DEPOT 1024
#define	SLOTP_TWO_HAND 2048

enum ItemTypes_t {
	ITEM_TYPE_NONE = 0,
	ITEM_TYPE_DEPOT,
	ITEM_TYPE_MAILBOX,
	ITEM_TYPE_TRASHHOLDER,
	ITEM_TYPE_CONTAINER,
	ITEM_TYPE_DOOR,
	ITEM_TYPE_MAGICFIELD,
	ITEM_TYPE_TELEPORT,
	ITEM_TYPE_BED,
	ITEM_TYPE_KEY,
	ITEM_TYPE_LAST
};

struct Abilities{
	Abilities();

	struct Absorb {
		int16_t resistances[COMBAT_COUNT];

		bool any() const;
		bool reduce(CombatType_t type, int32_t& dmg) const;
		std::ostream& getDescription(std::ostream& os) const;

	protected:
		std::ostream& getDescription(std::ostream& os, bool& first, unsigned int combat_type) const;
	} absorb;
	
	CombatType_t elementType;
	int16_t elementDamage;

	//extra skill modifiers
	int32_t skills[SKILL_LAST + 1];

	//stats modifiers
	int32_t stats[STAT_LAST + 1];
	int32_t statsPercent[STAT_LAST + 1];

	int32_t speed;
	bool manaShield;
	bool invisible;
	bool preventLoss;

	bool regeneration;
	uint32_t healthGain;
	uint32_t healthTicks;

	uint32_t manaGain;
	uint32_t manaTicks;

	uint32_t conditionImmunities;
	uint32_t conditionSuppressions;
};

class Condition;

class ItemType {
private:
	//It is private because calling it can cause unexpected results
	ItemType(const ItemType& it){};

public:
	ItemType();
	~ItemType();

	itemgroup_t group;
	ItemTypes_t type;

	bool isGroundTile() const {return (group == ITEM_GROUP_GROUND);}
	bool isContainer() const {return (group == ITEM_GROUP_CONTAINER);}
	bool isDoor() const {return (group == ITEM_GROUP_DOOR);}
	bool isTeleport() const {return (group == ITEM_GROUP_TELEPORT);}
	bool isMagicField() const {return (group == ITEM_GROUP_MAGICFIELD);}
	bool isSplash() const {return (group == ITEM_GROUP_SPLASH);}
	bool isFluidContainer() const {return (group == ITEM_GROUP_FLUID);}

	bool isKey() const {return (group == ITEM_GROUP_KEY);}
	bool isRune() const {return (group == ITEM_GROUP_RUNE);}
	bool isDepot() const {return (type == ITEM_TYPE_DEPOT);}
	bool isMailbox() const {return (type == ITEM_TYPE_MAILBOX);}
	bool isTrashHolder() const {return (type == ITEM_TYPE_TRASHHOLDER);}
	bool hasSubType() const {return (isFluidContainer() || isSplash() || stackable || charges != 0);}
	
	//[ added for beds system
	bool isBed() const {return type == ITEM_TYPE_BED;}

	Direction bedPartnerDir;
	uint16_t maleSleeperID;
	uint16_t femaleSleeperID;
	uint16_t noSleeperID;
	//]

	uint16_t id;
	uint16_t clientId;

	std::string    name;
	std::string    article;
	std::string    pluralName;
	std::string    description;
	uint16_t       maxItems;
	float          weight;
	bool           showCount;
	WeaponType_t   weaponType;
	Ammo_t         amuType;
	ShootType_t    shootType;
	MagicEffectClasses magicEffect;
	int32_t        attack;
	int32_t        defence;
	int32_t        extraDef;
	int32_t        armor;
	uint16_t       slot_position;
	bool           isVertical;
	bool           isHorizontal;
	bool           isHangable;
	bool           allowDistRead;
	bool           clientCharges;
	uint16_t       speed;
	int32_t        decayTo;
	uint32_t       decayTime;
	bool           stopTime;
	RaceType_t     corpseType;

	bool           canReadText;
	bool           canWriteText;
	uint16_t       maxTextLen;
	uint16_t       writeOnceItemId;

	bool           stackable;
	bool           useable;
	bool           moveable;
	bool           alwaysOnTop;
	int32_t        alwaysOnTopOrder;
	bool           pickupable;
	bool           rotable;
	int32_t        rotateTo;

	int32_t        runeMagLevel;
	int32_t        runeLevel;
	std::string    runeSpellName;

	uint32_t       wieldInfo;
	std::string    vocationString;
	uint32_t       minReqLevel;
	uint32_t       minReqMagicLevel;

	int32_t lightLevel;
	int32_t lightColor;

	bool floorChangeDown;
	bool floorChangeNorth;
	bool floorChangeSouth;
	bool floorChangeEast;
	bool floorChangeWest;
	bool hasHeight;

	bool blockSolid;
	bool blockPickupable;
	bool blockProjectile;
	bool blockPathFind;
	bool allowPickupable;

	unsigned short transformEquipTo;
	unsigned short transformDeEquipTo;
	bool showDuration;
	bool showCharges;
	uint32_t charges;
	int32_t breakChance;
	int32_t hitChance;
	int32_t maxHitChance;
	uint32_t shootRange;
	AmmoAction_t ammoAction;
	int32_t fluidSource;

	Abilities abilities;

	Condition* condition;
	CombatType_t combatType;
	bool replaceable;
};

template<typename A>
class Array{
public:
	Array(uint32_t n);
	~Array();

	A getElement(uint32_t id);
	const A getElement(uint32_t id) const;
	void addElement(A a, uint32_t pos);

	uint32_t size() {return m_size;}

private:
	A* m_data;
	uint32_t m_size;
};



class Items{
public:
	Items();
	~Items();

	bool reload();
	void clear();

	int loadFromOtb(std::string);

	const ItemType& operator[](int32_t id) const {return getItemType(id);}
	const ItemType& getItemType(int32_t id) const;
	ItemType& getItemType(int32_t id);
	const ItemType& getItemIdByClientId(int32_t spriteId) const;

	int32_t getItemIdByName(const std::string& name);

	static uint32_t dwMajorVersion;
	static uint32_t dwMinorVersion;
	static uint32_t dwBuildNumber;

	bool loadFromXml(const std::string& datadir);

	void addItemType(ItemType* iType);

	const ItemType* getElement(uint32_t id) const {return items.getElement(id);}
	uint32_t size() {return items.size();}

protected:
	typedef std::map<int32_t, int32_t> ReverseItemMap;
	ReverseItemMap reverseItemMap;

	Array<ItemType*> items;
	std::string m_datadir;
};

template<typename A>
inline Array<A>::Array(uint32_t n)
{
	m_data = (A*)malloc(sizeof(A)*n);
	memset(m_data, 0, sizeof(A)*n);
	m_size = n;
}

template<typename A>
inline Array<A>::~Array()
{
	free(m_data);
}

template<typename A>
inline A Array<A>::getElement(uint32_t id)
{
	if(id < m_size){
		return m_data[id];
	}
	else{
		return 0;
	}
}

template<typename A>
inline const A Array<A>::getElement(uint32_t id) const
{
	if(id < m_size){
		return m_data[id];
	}
	else{
		return 0;
	}
}

template<typename A>
inline void Array<A>::addElement(A a, uint32_t pos)
{
    #define INCREMENT 5000
	if(pos >= m_size){
		m_data = (A*)realloc(m_data, sizeof(A)*(pos + INCREMENT));
		memset(m_data + m_size, 0, sizeof(A)*(pos + INCREMENT - m_size));
		m_size = pos + INCREMENT;
	}
	m_data[pos] = a;
}

#endif
