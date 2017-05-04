TRUE = 1
FALSE = 0

LUA_ERROR = -1
LUA_NO_ERROR = 0

NORTH = 0
EAST = 1
SOUTH = 2
WEST = 3
SOUTHWEST = 4
SOUTHEAST = 5
NORTHWEST = 6
NORTHEAST = 7

COMBAT_FORMULA_UNDEFINED = 0
COMBAT_FORMULA_LEVELMAGIC = 1
COMBAT_FORMULA_SKILL = 2
COMBAT_FORMULA_VALUE = 3
COMBAT_FORMULA_DAMAGE = 3

CONDITION_PARAM_OWNER = 1
CONDITION_PARAM_TICKS = 2
CONDITION_PARAM_OUTFIT = 3
CONDITION_PARAM_HEALTHGAIN = 4
CONDITION_PARAM_HEALTHTICKS = 5
CONDITION_PARAM_MANAGAIN = 6
CONDITION_PARAM_MANATICKS = 7
CONDITION_PARAM_DELAYED = 8
CONDITION_PARAM_SPEED = 9
CONDITION_PARAM_LIGHT_LEVEL = 10
CONDITION_PARAM_LIGHT_COLOR = 11
CONDITION_PARAM_SOULGAIN = 12
CONDITION_PARAM_SOULTICKS = 13
CONDITION_PARAM_MINVALUE = 14
CONDITION_PARAM_MAXVALUE = 15
CONDITION_PARAM_STARTVALUE = 16
CONDITION_PARAM_TICKINTERVAL = 17
CONDITION_PARAM_FORCEUPDATE = 18
CONDITION_PARAM_SKILL_MELEE = 19
CONDITION_PARAM_SKILL_FIST = 20
CONDITION_PARAM_SKILL_CLUB = 21
CONDITION_PARAM_SKILL_SWORD = 22
CONDITION_PARAM_SKILL_AXE = 23
CONDITION_PARAM_SKILL_DISTANCE = 24
CONDITION_PARAM_SKILL_SHIELD = 25
CONDITION_PARAM_SKILL_FISHING = 26
CONDITION_PARAM_STAT_MAXHITPOINTS = 27
CONDITION_PARAM_STAT_MAXMANAPOINTS = 28
CONDITION_PARAM_STAT_SOULPOINTS = 29
CONDITION_PARAM_STAT_MAGICPOINTS = 30
CONDITION_PARAM_STAT_MAXHITPOINTSPERCENT = 31
CONDITION_PARAM_STAT_MAXMANAPOINTSPERCENT = 32
CONDITION_PARAM_STAT_SOULPOINTSPERCENT = 33
CONDITION_PARAM_STAT_MAGICPOINTSPERCENT = 34
CONDITION_PARAM_PERIODICDAMAGE = 35

COMBAT_PARAM_TYPE = 1
COMBAT_PARAM_EFFECT = 2
COMBAT_PARAM_DISTANCEEFFECT = 3
COMBAT_PARAM_BLOCKSHIELD = 4
COMBAT_PARAM_BLOCKARMOR = 5
COMBAT_PARAM_TARGETCASTERORTOPMOST = 6
COMBAT_PARAM_CREATEITEM = 7
COMBAT_PARAM_AGGRESSIVE = 8
COMBAT_PARAM_DISPEL = 9

CALLBACK_PARAM_LEVELMAGICVALUE = 1
CALLBACK_PARAM_SKILLVALUE = 2
CALLBACK_PARAM_TARGETTILE = 3
CALLBACK_PARAM_TARGETCREATURE = 4

COMBAT_NONE = 0
COMBAT_PHYSICALDAMAGE = 1
COMBAT_ENERGYDAMAGE = 2
COMBAT_POISONDAMAGE = 4 
COMBAT_FIREDAMAGE = 8
COMBAT_UNDEFINEDDAMAGE = 16
COMBAT_LIFEDRAIN = 32
COMBAT_MANADRAIN = 64
COMBAT_HEALING = 128

CONDITION_NONE = 0
CONDITION_POISON = 1
CONDITION_FIRE = 2
CONDITION_ENERGY = 4
CONDITION_LIFEDRAIN = 8
CONDITION_HASTE = 16
CONDITION_PARALYZE = 32
CONDITION_OUTFIT = 64
CONDITION_INVISIBLE = 128
CONDITION_LIGHT = 256
CONDITION_MANASHIELD = 512
CONDITION_INFIGHT = 1024
CONDITION_DRUNK = 2048
CONDITION_EXHAUSTED = 4096
CONDITION_EXHAUST_COMBAT = 131072
CONDITION_FOOD = 8192
CONDITION_REGENERATION = 8192
CONDITION_SOUL = 16384
CONDITION_MUTED = 32768
CONDITION_ATTRIBUTES = 65536

PLAYERLOSS_EXPERIENCE = 0
PLAYERLOSS_MANA = 1
PLAYERLOSS_SKILL = 2

CONST_SLOT_HEAD = 1
CONST_SLOT_NECKLACE = 2
CONST_SLOT_BACKPACK = 3
CONST_SLOT_ARMOR = 4
CONST_SLOT_RIGHT = 5
CONST_SLOT_LEFT = 6
CONST_SLOT_LEGS = 7
CONST_SLOT_FEET = 8
CONST_SLOT_RING = 9
CONST_SLOT_AMMO = 10

CONST_ME_DRAWBLOOD = 0
CONST_ME_LOSEENERGY = 1
CONST_ME_POFF = 2
CONST_ME_BLOCKHIT = 3
CONST_ME_EXPLOSIONAREA = 4
CONST_ME_EXPLOSIONHIT = 5
CONST_ME_FIREAREA = 6
CONST_ME_YELLOW_RINGS = 7
CONST_ME_GREEN_RINGS = 8
CONST_ME_HITAREA = 9
CONST_ME_ENERGYAREA = 10
CONST_ME_ENERGYHIT = 11
CONST_ME_MAGIC_BLUE = 12
CONST_ME_MAGIC_RED = 13
CONST_ME_MAGIC_GREEN = 14
CONST_ME_HITBYFIRE = 15
CONST_ME_HITBYPOISON = 16
CONST_ME_MORTAREA = 17
CONST_ME_SOUND_BLUE = 18
CONST_ME_SOUND_RED = 19
CONST_ME_POISONAREA = 20
CONST_ME_SOUND_YELLOW = 21
CONST_ME_SOUND_PURPLE = 22
CONST_ME_SOUND_BLUE = 23
CONST_ME_SOUND_WHITE = 24
CONST_ME_NONE = 255

CONST_ANI_SPEAR = 0
CONST_ANI_BOLT = 1
CONST_ANI_ARROW = 2
CONST_ANI_FIRE = 3
CONST_ANI_ENERGY = 4
CONST_ANI_POISONARROW = 5
CONST_ANI_BURSTARROW = 6
CONST_ANI_THROWINGSTAR = 7
CONST_ANI_THROWINGKNIFE = 8
CONST_ANI_SMALLSTONE = 9
CONST_ANI_SUDDENDEATH = 10
CONST_ANI_LARGEROCK = 11
CONST_ANI_SNOWBALL = 12
CONST_ANI_POWERBOLT = 13
CONST_ANI_POISON = 14
CONST_ANI_NONE = 255

TALKTYPE_SAY  = 1
TALKTYPE_WHISPER = 2
TALKTYPE_YELL = 3
TALKTYPE_BROADCAST = 11
TALKTYPE_ORANGE_1 = 18
TALKTYPE_ORANGE_2 = 19
TALKTYPE_ORANGE = TALKTYPE_ORANGE_1
TALKTYPE_ORANGE_YELL = TALKTYPE_ORANGE_2

MESSAGE_STATUS_CONSOLE_RED = 17
MESSAGE_STATUS_CONSOLE_ORANGE = 19
MESSAGE_STATUS_WARNING = 20
MESSAGE_EVENT_ADVANCE = 21
MESSAGE_EVENT_DEFAULT = 22
MESSAGE_STATUS_DEFAULT = 23
MESSAGE_INFO_DESCR = 24
MESSAGE_STATUS_SMALL = 25
MESSAGE_STATUS_CONSOLE_BLUE = 26

TEXTCOLOR_BLUE        = 5
TEXTCOLOR_LIGHTBLUE   = 35
TEXTCOLOR_LIGHTGREEN  = 30
TEXTCOLOR_LIGHTGREY   = 129
TEXTCOLOR_RED         = 180
TEXTCOLOR_ORANGE      = 198
TEXTCOLOR_WHITE_EXP   = 215
TEXTCOLOR_NONE        = 255

ITEM_TYPE_NONE        = 0
ITEM_TYPE_DEPOT       = 1
ITEM_TYPE_MAILBOX     = 2
ITEM_TYPE_TRASHHOLDER = 3
ITEM_TYPE_CONTAINER   = 4
ITEM_TYPE_DOOR        = 5
ITEM_TYPE_MAGICFIELD  = 6
ITEM_TYPE_TELEPORT    = 7
ITEM_TYPE_BED         = 8

CONST_PROP_BLOCKSOLID = 0
CONST_PROP_HASHEIGHT = 1
CONST_PROP_BLOCKPROJECTILE = 2
CONST_PROP_BLOCKPATHFIND = 3
CONST_PROP_ISVERTICAL = 4
CONST_PROP_ISHORIZONTAL = 5
CONST_PROP_MOVEABLE = 6
CONST_PROP_BLOCKINGANDNOTMOVEABLE = 7
CONST_PROP_SUPPORTHANGABLE = 8

RETURNVALUE_NOERROR = 1
RETURNVALUE_NOTPOSSIBLE = 2
RETURNVALUE_NOTENOUGHROOM = 3
RETURNVALUE_PLAYERISPZLOCKED = 4
RETURNVALUE_PLAYERISNOTINVITED = 5
RETURNVALUE_CANNOTTHROW = 6
RETURNVALUE_THEREISNOWAY = 7
RETURNVALUE_DESTINATIONOUTOFREACH = 8
RETURNVALUE_CREATUREBLOCK = 9
RETURNVALUE_NOTMOVEABLE = 10
RETURNVALUE_DROPTWOHANDEDITEM = 11
RETURNVALUE_BOTHHANDSNEEDTOBEFREE = 12
RETURNVALUE_CANONLYUSEONEWEAPON = 13
RETURNVALUE_NEEDEXCHANGE = 14
RETURNVALUE_CANNOTBEDRESSED = 15
RETURNVALUE_PUTTHISOBJECTINYOURHAND = 16
RETURNVALUE_PUTTHISOBJECTINBOTHHANDS = 17
RETURNVALUE_TOOFARAWAY = 18
RETURNVALUE_FIRSTGODOWNSTAIRS = 19
RETURNVALUE_FIRSTGOUPSTAIRS = 20
RETURNVALUE_CONTAINERNOTENOUGHROOM = 21
RETURNVALUE_NOTENOUGHCAPACITY = 22
RETURNVALUE_CANNOTPICKUP = 23
RETURNVALUE_THISISIMPOSSIBLE = 24
RETURNVALUE_DEPOTISFULL = 25
RETURNVALUE_CREATUREDOESNOTEXIST = 26
RETURNVALUE_CANNOTUSETHISOBJECT = 27
RETURNVALUE_PLAYERWITHTHISNAMEISNOTONLINE = 28
RETURNVALUE_NOTREQUIREDLEVELTOUSERUNE = 29
RETURNVALUE_YOUAREALREADYTRADING = 30
RETURNVALUE_THISPLAYERISALREADYTRADING = 31
RETURNVALUE_YOUMAYNOTLOGOUTDURINGAFIGHT = 32
RETURNVALUE_DIRECTPLAYERSHOOT = 33
RETURNVALUE_NOTENOUGHLEVEL = 34
RETURNVALUE_NOTENOUGHMAGICLEVEL = 35
RETURNVALUE_NOTENOUGHMANA = 36
RETURNVALUE_NOTENOUGHSOUL = 37
RETURNVALUE_YOUAREEXHAUSTED = 38
RETURNVALUE_PLAYERISNOTREACHABLE = 39
RETURNVALUE_CANONLYUSETHISRUNEONCREATURES = 40
RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE = 41
RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER = 42
RETURNVALUE_YOUMAYNOTATTACKAPERSONINPROTECTIONZONE = 43
RETURNVALUE_YOUMAYNOTATTACKAPERSONWHILEINPROTECTIONZONE = 44
RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE = 45
RETURNVALUE_YOUCANONLYUSEITONCREATURES = 46
RETURNVALUE_CREATUREISNOTREACHABLE = 47
RETURNVALUE_TURNSECUREMODETOATTACKUNMARKEDPLAYERS = 48
RETURNVALUE_YOUNEEDPREMIUMACCOUNT = 49
RETURNVALUE_YOUNEEDTOLEARNTHISSPELL = 50
RETURNVALUE_YOURVOCATIONCANNOTUSETHISSPELL = 51
RETURNVALUE_YOUNEEDAWEAPONTOUSETHISSPELL = 52
RETURNVALUE_PLAYERISPZLOCKEDLEAVEPVPZONE = 53
RETURNVALUE_PLAYERISPZLOCKEDENTERPVPZONE = 54
RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE = 55
RETURNVALUE_YOUCANNOTLOGOUTHERE = 56

ITEM_GOLD_COIN = 2148
ITEM_PLATINUM_COIN = 2152
ITEM_CRYSTAL_COIN = 2160
ITEM_GOLD_INGOT = 9971

function doPlayerGiveItem(cid, itemid, count, charges)
	local hasCharges = (isItemRune(itemid) == TRUE or isItemFluidContainer(itemid) == TRUE)
	if(hasCharges and charges == nil) then
		charges = 1
	end
 
	while count > 0 do
    	local tempcount = 1
 
    	if(hasCharges) then
    		tempcount = charges
    	end
    	if(isItemStackable(itemid) == TRUE) then
    		tempcount = math.min (100, count)
   		end
 
       	local ret = doPlayerAddItem(cid, itemid, tempcount)
       	if(ret == LUA_ERROR) then
        	ret = doCreateItem(itemid, tempcount, getPlayerPosition(cid))
        end
 
        if(ret ~= LUA_ERROR) then
        	if(hasCharges) then
        		count = count-1
        	else
        		count = count-tempcount
        	end
        else
        	return LUA_ERROR
        end
	end
    return LUA_NO_ERROR
end

function doPlayerTakeItem(cid, itemid, count)
	if(getPlayerItemCount(cid,itemid) >= count) then
 
		while count > 0 do
			local tempcount = 0
    		if(isItemStackable(itemid) == TRUE) then
    			tempcount = math.min (100, count)
    		else
    			tempcount = 1
    		end
        	local ret = doPlayerRemoveItem(cid, itemid, tempcount)
 
            if(ret ~= LUA_ERROR) then
            	count = count-tempcount
            else
            	return LUA_ERROR
            end
		end
 
		if(count == 0) then
			return LUA_NO_ERROR
		end
 
	else
		return LUA_ERROR
	end
end
 
 
function doPlayerAddMoney(cid, amount)
	local crystals = math.floor(amount/10000)
	amount = amount - crystals*10000
	local platinum = math.floor(amount/100)
	amount = amount - platinum*100
	local gold = amount
	local ret = 0
	if(crystals > 0) then
		ret = doPlayerGiveItem(cid, ITEM_CRYSTAL_COIN, crystals)
		if(ret ~= LUA_NO_ERROR) then
			return LUA_ERROR
		end
	end
	if(platinum > 0) then
		ret = doPlayerGiveItem(cid, ITEM_PLATINUM_COIN, platinum)
		if(ret ~= LUA_NO_ERROR) then
			return LUA_ERROR
		end
	end
	if(gold > 0) then
		ret = doPlayerGiveItem(cid, ITEM_GOLD_COIN, gold)
		if(ret ~= LUA_NO_ERROR) then
			return LUA_ERROR
		end
	end
	return LUA_NO_ERROR
end
 
 
function doPlayerBuyItem(cid, itemid, count, cost, charges)
    if(doPlayerRemoveMoney(cid, cost) == TRUE) then
    	return doPlayerGiveItem(cid, itemid, count, charges)
    else
        return LUA_ERROR
    end
end
 
 
function doPlayerSellItem(cid, itemid, count, cost)
 
	if(doPlayerTakeItem(cid, itemid, count) == LUA_NO_ERROR) then
		if(doPlayerAddMoney(cid, cost) ~= LUA_NO_ERROR) then
			error('Could not add money to ' .. getPlayerName(cid) .. '(' .. cost .. 'gp)')
		end
		return LUA_NO_ERROR
	else
		return LUA_ERROR
	end
 
end

function string.explode(str, delimiter)
	if str == nil then
		return {}
	end
	t = {}
	for v in string.gmatch(str, "([^,]*)" .. delimiter .. "?") do
		table.insert(t, v)
	end
	table.remove(t) -- Removes last element (Always "")
	return t
end

function string.strip_whitespace(str)
	if str == nil then return str end
	local start = string.find(str, "[^%s]") -- First non-whitespace character
	local _end = #str + 1 - string.find(str:reverse(), "[^%s]") -- Last non-whitespace character
	
	if start ~= nil and _end ~= nil then
		return string.sub(str, start, _end)
	elseif start ~= nil then
		return string.sub(str, start)
	elseif _end ~= nil then
		return string.sub(str, 1, _end)
	end
	return str
end

function getPlayerByAccountNumber(acc)
	players = getPlayersByAccountNumber(acc)
	if #players == 0 then
		return 0
	end
	return players[1]
end

function convertIntToIP(int, mask)
	local b4 = bit.urshift(bit.uband(int,  4278190080), 24)
	local b3 = bit.urshift(bit.uband(int,  16711680), 16)
	local b2 = bit.urshift(bit.uband(int,  65280), 8)
	local b1 = bit.urshift(bit.uband(int,  255), 0)
	if mask ~= nil then
		local m4 = bit.urshift(bit.uband(mask,  4278190080), 24)
		local m3 = bit.urshift(bit.uband(mask,  16711680), 16)
		local m2 = bit.urshift(bit.uband(mask,  65280), 8)
		local m1 = bit.urshift(bit.uband(mask,  255), 0)
		if (m1 == 255 or m1 == 0) and (m2 == 255 or m2 == 0) and (m3 == 255 or m3 == 0) and (m4 == 255 or m4 == 0) then
			if m1 == 0 then b1 = "x" end
			if m2 == 0 then b2 = "x" end
			if m3 == 0 then b3 = "x" end
			if m4 == 0 then b4 = "x" end
		else
			if m1 ~= 255 or m2 ~= 255 or m3 ~= 255 or m4 ~= 255 then
				return b1 .. "." .. b2 .. "." .. b3 .. "." .. b4 .. " : " .. m1 .. "." .. m2 .. "." .. m3 .. "." .. m4
			end
		end
	end
	
	return b1 .. "." .. b2 .. "." .. b3 .. "." .. b4
end

function convertIPToInt(str)
	local maskindex = str:find(":")
	if maskindex ~= nil then
		-- IP:Mask style
		if maskindex <= 1 then
			return 0, 0
		else
			ipstring = str:sub(1, maskindex - 1)
			maskstring = str:sub(maskindex)
			
			local ipint = 0
			local maskint = 0
			
			local index = 0
			for b in ipstring:gmatch("(%d+).?") do
				if tonumber(b) > 255 or tonumber(b) < 0 then
					return 0, 0
				end
				ipint = bit.ubor(ipint, bit.ulshift(b, index))
				index = index + 8
				if index > 24 then
					break
				end
			end
			if index ~= 32 then -- Invalid
				return 0, 0
			end
			
			index = 0
			for b in maskstring:gmatch("(%d+)%.?") do
				if tonumber(b) > 255 or tonumber(b) < 0 then
					return 0, 0
				end
				maskint = bit.ubor(maskint, bit.ulshift(b, index))
				index = index + 8
				if index > 24 then
					break
				end
			end
			if index ~= 32 then
				return 0, 0
			end
			
			return ipint, maskint
		end
	else
		local ipint = 0
		local maskint = 0
		local index = 24
		
		for b in str:gmatch("([x%d]+)%.?") do
			if b ~= "x" then
				if b:find("x") ~= nil then
					return 0, 0
				end
				if tonumber(b) > 255 or tonumber(b) < 0 then
					return 0, 0
				end
				maskint = bit.ubor(maskint, bit.ulshift(255, index))
				ipint = bit.ubor(ipint, bit.ulshift(b, index))
			end
			index = index - 8
			if index < 0 then
				break
			end
		end
		if index ~= -8 then -- Invalid
			return 0, 0
		end
		return ipint, maskint
	end
end

function getDirectionTo(pos1, pos2)
	local dir = NORTH
	if(pos1.x > pos2.x) then
		dir = WEST
		if(pos1.y > pos2.y) then
			dir = NORTHWEST
		elseif(pos1.y < pos2.y) then
			dir = SOUTHWEST
		end
	elseif(pos1.x < pos2.x) then
		dir = EAST
		if(pos1.y > pos2.y) then
			dir = NORTHEAST
		elseif(pos1.y < pos2.y) then
			dir = SOUTHEAST
		end
	else
		if(pos1.y > pos2.y) then
			dir = NORTH
		elseif(pos1.y < pos2.y) then
			dir = SOUTH
		end
	end
	return dir
end

--Task System


--Task system
task_sys = {
	[1] = {name = "Rats", start = 176201, monsters_list = {"Rat","Cave rat"}, level = 8, count = 100, points = 0, items = {}, reward = {}, exp = 0, money = 2000},
	[2] = {name = "Trolls", start = 176202, monsters_list = {"Troll", "Frost Troll"}, level = 10, count = 100, points = 0, items = {}, reward = {}, exp = 4000, money = 0},
	[3] = {name = "Swamp Trolls", start = 176203, monsters_list = {"Swamp troll"}, level = 11, count = 100, points = 0, items = {}, reward = {}, exp = 5000, money = 0},
	[4] = {name = "Snakes", start = 176204, monsters_list = {"Snake"}, level = 12, count = 100, points = 1, items = {}, reward = {}, exp = 0, money = 4000},
	[5] = {name = "Hyenas", start = 176205, monsters_list = {"Hyena"}, level = 12, count = 150, points = 1, items = {}, reward = {}, exp = 8000, money = 0},
	[6] = {name = "Rotworms", start = 176206, monsters_list = {"Rotworm"}, level = 13, count = 500, points = 2, items = {}, reward = {}, exp = 25000, money = 5000},
	[7] = {name = "Orcs", start = 176207, monsters_list = {"Orc", "Orc spearman", "Orc warrior"}, level = 13, count = 500, points = 2, items = {{5139,1}}, reward = {}, exp = 27000, money = 2000},
	[8] = {name = "Skeletons", start = 176208, monsters_list = {"Skeleton"}, level = 15, count = 100, points = 2, items = {}, reward = {{5136,1}}, exp = 10000, money = 0},
	[9] = {name = "Amazons", start = 176209, monsters_list = {"Amazon"}, level = 20, count = 200, points = 2, items = {}, reward = {{5140,1}}, exp = 2400, money = 10000},
	[10] = {name = "Valkyries", start = 176210, monsters_list = {"Valkyrie"}, level = 25, count = 200, points = 2, items = {}, reward = {{5137,1}}, exp = 0, money = 10000},	
	[11] = {name = "Witches", start = 176211, monsters_list = {"Witch"}, level = 30, count = 50, points = 2, items = {}, reward = {}, exp = 24000, money = 0},	
	[12] = {name = "Elfs scoults", start = 176212, monsters_list = {"Elf scoult"}, level = 32, count = 150, points = 2, items = {}, reward = {}, exp = 40000, money = 3000},		
	[13] = {name = "Cyclops", start = 176213, monsters_list = {"Cyclops"}, level = 33, count = 150, points = 2, items = {}, reward = {}, exp = 100000, money = 5000},	
	[14] = {name = "Crypt Shamblers", start = 176214, monsters_list = {"Crypt Shambler"}, level = 36, count = 400, points = 3, items = {}, reward = {}, exp = 150000, money = 8000},
	[15] = {name = "Dwarf Guards", start = 176215, monsters_list = {"Dwarf Guard"}, level = 38, count = 650, points = 3, items = {}, reward = {}, exp = 45000, money = 8000},
	[16] = {name = "Minotaur mages", start = 176216, monsters_list = {"Minotaur mage"}, level = 40, count = 200, points = 2, items = {}, reward = {}, exp = 100000, money = 8200},
	[17] = {name = "Demon Skeletons", start = 176217, monsters_list = {"Demon Skeleton"}, level = 42, count = 400, points = 3, items = {}, reward = {}, exp = 200000, money = 0},
	[18] = {name = "Barbarians", start = 176218, monsters_list = {"Barbarian Skullhunter", "Barbarian Headsplitter", "Barbarian Brutetamer", "Barbarian Bloodwalker"}, level = 44, count = 170, points = 2, items = {}, reward = {}, exp = 200000, money = 5000},
	[19] = {name = "Liches", start = 176219, monsters_list = {"Lich"}, level = 46, count = 170, points = 2, items = {}, reward = {}, exp = 200000, money = 5000},
	[20] = {name = "Bonebeasts", start = 176220, monsters_list = {"Bonebeast"}, level = 50, count = 400, points = 1, items = {}, reward = {}, exp = 200000, money = 9000},
	[21] = {name = "Minotaurs" ,start = 176221, monsters_list = {"Minotaur", "Minotaur mage", "Minotaur archer", "Minotaur guard"}, level = 60, count = 4000, points = 3, items = {}, reward = {{5127,1}}, exp = 250000, money = 15000},
	[22] = {name = "Crystal Spiders" ,start = 176222, monsters_list = {"Crystal Spider"}, level = 63, count = 250, points = 3, items = {}, reward = {}, exp = 25000, money = 11000},
	[23] = {name = "Giant Spider" ,start = 176223, monsters_list = {"Giant Spider","The old widow"}, level = 500, count = 500, points = 0, items = {}, reward = {}, exp = 20000, money = 32000},
	[24] = {name = "Dragons" ,start = 176224, monsters_list = {"Dragon"}, level = 82, count = 2500, points = 3, items = {}, reward = {}, exp = 1000000, money = 5000},
	[25] = {name = "Ancient Scarabs" ,start = 176225, monsters_list = {"Ancient Scarab"}, level = 83, count = 600, points = 3, items = {}, reward = {}, exp = 25000, money = 18000},
	[26] = {name = "Vampires" ,start = 176226, monsters_list = {"Vampire"}, level = 85, count = 450, points = 4, items = {}, reward = {}, exp = 250000, money = 10000},
	[27] = {name = "Heroes" ,start = 176227, monsters_list = {"Hero"}, level = 88, count = 200, points = 3, items = {}, reward = {}, exp = 250000, money = 11000},
	[28] = {name = "Dragon Lords" ,start = 176228, monsters_list = {"Dragon lord"}, level = 89, count = 500, points = 3, items = {}, reward = {}, exp = 400000, money = 8000},
	[29] = {name = "Dragon Lords" ,start = 176229, monsters_list = {"Black hunter"}, level = 90, count = 100, points = 3, items = {}, reward = {{5092, 1}}, exp = 450000, money = 0},	
	[30] = {name = "Warlocks" ,start = 17630, monsters_list = {"Warlock"}, level = 91, count = 600, points = 4, items = {}, reward = {}, exp = 400000, money = 10000},
	[31] = {name = "Hydras" ,start = 176231, monsters_list = {"Hydra"}, level = 100, count = 700, points = 3, items = {}, reward = {}, exp = 0, money = 40000},
	[32] = {name = "Serpent Spawn" ,start = 176232, monsters_list = {"Serpent Spawn"}, level = 103, count = 720, points = 4, items = {}, reward = {}, exp = 50000, money = 45000},
	[33] = {name = "Demons" ,start = 176233, monsters_list = {"Demon"}, level = 110, count = 666, points = 4, items = {}, reward = {}, exp = 666666, money = 66666}
}

daily_task = {
	[1] = {name = "Medium Orcs" ,monsters_list = {"Orc Rider","Orc Leader","Orc Warlord"}, count = 2 , points = 0, reward = {}, exp = 5000, money = 1000},
	[2] = {name = "Tarantulas" ,monsters_list = {"Tarantula"}, count = 120, points = 1, reward = {}, exp = 9000, money = 1500},
	[3] = {name = "Barbarians" ,monsters_list = {"Barbarian Skullhunter", "Barbarian Headsplitter", "Barbarian Brutetamer", "Barbarian Bloodwalker"}, count = 20, points = 0, reward = {}, exp = 9000, money = 1500},
	[4] = {name = "Dragons" ,monsters_list = {"Dragon"}, count = 50, points = 0, reward = {}, exp = 2000, money = 3000},
	[5] = {name = "Liches" ,monsters_list = {"Lich"}, count = 8, points = 0, reward = {}, exp = 2000, money = 2000},	
	[6] = {name = "Ancient Scarabs" ,monsters_list = {"Ancient Scarab"}, count = 80, points = 5, reward = {}, exp = 22000, money = 2000},
	[7] = {name = "Dragon Lords" ,monsters_list = {"Dragon lord"}, count = 10, points = 5, reward = {}, exp = 22000, money = 2000},	
	[8] = {name = "Behemoths" ,monsters_list = {"Behemoth"}, count = 8, points = 5, reward = {}, exp = 33000, money = 2000},		
	[9] = {name = "Giant Spiders" ,monsters_list = {"Giant Spider"}, count = 15, points = 3, reward = {}, exp = 15000, money = 3000},
	[10] = {name = "Black hunter" ,monsters_list = {"Black hunter"}, count = 18, points = 2, reward = {{5151,1}}, exp = 20000, money = 8000},
	[11] = {name = "Hydras" ,monsters_list = {"Hydra"}, count = 20, points = 10, reward = {{2173,1}}, exp = 20000, money = 8000},
	
}
task_sys_storages = {176601, 176602, 176603, 176604, 176605, 176606, 176607, 176608} -- task, points, count, daily task, daily count, daily time , daily start, contador
function getTaskMission(cid)
	return getPlayerStorageValue(cid,task_sys_storages[1]) < 0 and 1 or getPlayerStorageValue(cid,task_sys_storages[1])
end
function getDailyTaskMission(cid)
	return getPlayerStorageValue(cid,task_sys_storages[4]) < 0 and 1 or getPlayerStorageValue(cid,task_sys_storages[4])
end
function getTaskPoints(cid)
	return getPlayerStorageValue(cid,task_sys_storages[2]) < 0 and 0 or getPlayerStorageValue(cid,task_sys_storages[2])
end
function doRandomDailyTask(cid)
	local t = {
		[{6,49}] = {1,4},
		[{50,79}] = {6,9},
		[{80,129}] = {7,12},
		[{130,math.huge}] = {10,12}
	}
	for a , b in pairs(t) do
		if getPlayerLevel(cid) >= a[1] and getPlayerLevel(cid) <= a[2] then
			return math.random(b[1], b[2])
		end
	end
	return 0
end
function GetRankTask(cid)
	local ranks = {
		[{1, 20}] = "Huntsman", 
		[{21, 50}] = "Ranger",
		[{51, 100}] = "Big Game Hunter",
		[{101, 200}] = "Trophy Hunter",		
		[{201, math.huge}] = "Elite Hunter"
	}
	for v , r in pairs(ranks) do
		if getTaskPoints(cid) >= v[1] and getTaskPoints(cid) <= v[2] then
			return r
		end
	end
	return 0
end
function getItemsFromList(items)
	local str = ''
	if table.maxn(items) > 0 then
		for i = 1, table.maxn(items) do
			str = str .. items[i][2] .. ' ' .. getItemNameById(items[i][1])
			if i ~= table.maxn(items) then str = str .. ', ' 
			end 
		end 
	end
	return str
end
function doRemoveItemsFromList(cid,items)
	local count = 0
	if table.maxn(items) > 0 then
		for i = 1, table.maxn(items) do
			if getPlayerItemCount(cid,items[i][1]) >= items[i][2] then
			count = count + 1 end 
		end 
	end
	if count == table.maxn(items) then
		for i = 1, table.maxn(items) do doPlayerRemoveItem(cid,items[i][1],items[i][2]) end
	else 
		return false 
	end
	return true 
end
function getMonsterFromList(monster)
	local str = ''
	if #monster > 0 then
		for i = 1, #monster do
			str = str .. monster[i]
			if i ~= #monster then str = str .. ', ' end
		end 
	end
	return str
end
function GiveRewardsTask(cid, items)
	local backpack = doPlayerAddItem(cid, 1999, 1) -- backpackID
	for _, i_i in ipairs(items) do
		local item, amount = i_i[1],i_i[2]
		if isItemStackable(item) or amount == 1 then
			doAddContainerItem(backpack, item, amount)
		else
			for i = 1, amount do
				doAddContainerItem(backpack, item, 1)
			end
		end
	end
end

