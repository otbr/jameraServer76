local exhaust_heal = createConditionObject(CONDITION_EXHAUSTED)
setConditionParam(exhaust_heal, CONDITION_PARAM_TICKS, 1000)

local poison = createConditionObject(CONDITION_POISON)
setConditionParam(poison, CONDITION_PARAM_DELAYED, 1)

local drunk = createConditionObject(CONDITION_DRUNK)
setConditionParam(drunk, CONDITION_PARAM_TICKS, 60000)
addDamageCondition(poison, 4, 6000, -5)
addDamageCondition(poison, 5, 6000, -4)
addDamageCondition(poison, 6, 6000, -3)
addDamageCondition(poison, 10, 6000, -2)
addDamageCondition(poison, 36, 6000, -1)

local TYPE_EMPTY = 0
local TYPE_WATER = 1
local TYPE_BLOOD = 2
local TYPE_BEER = 3
local TYPE_SLIME = 4
local TYPE_LEMONADE = 5
local TYPE_MANA_FLUID = 7
local TYPE_LIFE_FLUID = 10
local TYPE_OIL = 11
local TYPE_URINE = 13
local TYPE_WINE = 15
local TYPE_MUD = 19
local TYPE_LAVA = 26
local TYPE_RUM = 27
local TYPE_SWAMP = 28

function onUse(cid, item, frompos, item2, topos)

        if (item.type == 0) then
                  doPlayerSendCancel(cid,"You can not use this object. ")
        elseif (item2.uid ~= cid) and (topos.stackpos > 0) and (isMoveable(item2.uid) ~= TRUE) and (isCreature(item2.uid) ~= 1) then
                  doPlayerSendCancel(cid,"There is not enough room.")
    elseif (isInArray(FLUID_CONTAINERS,item2.itemid) == TRUE) and (item2.type == TYPE_EMPTY) then
          doChangeTypeItem(item.uid, TYPE_EMPTY)
                  doChangeTypeItem(item2.uid, item.type)

            elseif (topos.x == 65535) and (item2.uid ~= cid) and (isPlayer(item2.uid) ~= cid) then
                if(isInArray(DIRT_WALL,item2.itemid) ~= TRUE) then
          doChangeTypeItem(item.uid, 0)
                  splash = doCreateItem(2025, item.type, getThingPos(cid))
          doDecayItem(splash)
                else
                  doPlayerSendCancel(cid,"There is not enough room.")
                end
            elseif (item2.itemid > 0) and (item2.uid ~= cid) and (isPlayer(item2.uid) ~= cid) then
                if(isInArray(DIRT_WALL,item2.itemid) ~= TRUE) then
          doChangeTypeItem(item.uid, 0)
                  splash = doCreateItem(2025, item.type, topos)
                  if(isInArray(WATER_LAVA_SWAMP,item2.itemid) ~= TRUE) then
                  doDecayItem(splash)
                  end
                else
                  doPlayerSendCancel(cid,"There is not enough room.")
                end
            elseif (frompos.x == 65535) and (item2.uid ~= cid) and (isPlayer(item2.uid) ~= cid) then
                if(isInArray(DIRT_WALL,item2.itemid) ~= TRUE) then
          doChangeTypeItem(item.uid, 0)
                  splash = doCreateItem(2025, item.type, getThingPos(cid))
          doDecayItem(splash)
                else
                  doPlayerSendCancel(cid,"There is not enough room.")
                end
        else
        if (item2.uid == cid) and (isPlayer(item2.uid) == 1) then
              doChangeTypeItem(item.uid, 0)
              if (item.type == TYPE_MANA_FLUID) then
          new_mana = math.random(80, 160)
          doPlayerAddMana(cid, new_mana)
          doSendMagicEffect(topos, 12)
          doPlayerSay(cid, "Aaaah...", 1)
                  doAddCondition(cid, exhaust_heal)
              elseif (item.type == TYPE_LIFE_FLUID) then
          new_life = math.random(20, 50)
          doPlayerAddHealth(cid, new_life)
              doSendMagicEffect(topos, 12)
          doPlayerSay(cid, "Aaaah...", 1)
                  doAddCondition(cid, exhaust_heal)
          elseif (item.type == TYPE_BEER or item.type == TYPE_WINE or item.type == TYPE_RUM) then
                  doTargetCombatCondition(0, cid, drunk, CONST_ME_NONE)
              doPlayerSay(cid, "Aah...", 1)
          elseif (item.type == TYPE_LEMONADE) then
              doPlayerSay(cid, "Mmmh.", 1)
              elseif (item.type == TYPE_SLIME or item.type == TYPE_SWAMP) then
                  doTargetCombatCondition(0, cid, poison, CONST_ME_NONE)
          doPlayerSay(cid, "Urgh!", 1)
              elseif (item.type == TYPE_OIL or item.type == TYPE_URINE) then
          doPlayerSay(cid, "Urgh!", 1)
          else
          doPlayerSay(cid, "Gulp.", 1)
              end
            end
        end
    if (item.type == 0) then
    if item2.itemid >= 490 and item2.itemid <= 517
    or item2.itemid >= 618 and item2.itemid <= 629
    or item2.itemid == 1368 or item2.itemid == 1369
    or item2.itemid >= 4820 and item2.itemid <= 4825
    or item2.itemid >= 4828 and item2.itemid <= 4831
    or item2.itemid >= 4608 and item2.itemid <= 4666 then
        doChangeTypeItem(item.uid, TYPE_WATER)
    elseif item2.itemid == 103 then
        doChangeTypeItem(item.uid, TYPE_MUD)
    elseif item2.itemid >= 598 and item2.itemid < 601
    or item2.itemid == 1509
    or item2.itemid >= 518 and item2.itemid <= 529 then
        doChangeTypeItem(item.uid, TYPE_LAVA)
    elseif item2.itemid >= 351 and item2.itemid <= 355 then
        doChangeTypeItem(item.uid, TYPE_MUD)
    elseif item2.itemid >= 602 and item2.itemid <= 605
    or item2.itemid >= 4691 and item2.itemid <= 4755
    or item2.itemid == 4758 then
        doChangeTypeItem(item.uid, TYPE_SWAMP)
    elseif item2.itemid == 1771 then
        doChangeTypeItem(item.uid, TYPE_WATER)
    elseif item2.itemid == 1772 then
        doChangeTypeItem(item.uid, TYPE_BEER)
    elseif item2.itemid == 1773 then
        doChangeTypeItem(item.uid, TYPE_WINE)
    elseif item2.itemid == 1776 then
        doChangeTypeItem(item.uid, TYPE_LEMONADE)
        elseif(isInArray(NORMAL_CORPSE_STAGE_I,item2.itemid) == TRUE) then
        doChangeTypeItem(item.uid, TYPE_BLOOD)
        elseif(isInArray(SLIME_CORPSES,item2.itemid) == TRUE) then
        doChangeTypeItem(item.uid, TYPE_SLIME)
    elseif item2.itemid >= 371 and item2.itemid <= 382
            or item2.itemid >= 356 and item2.itemid <= 367 then
            doPlayerSendCancel(cid, "You cannot use this object.")
        end
    end
  return TRUE
end