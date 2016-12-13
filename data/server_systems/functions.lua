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

function getDistanceBetween(pos1, pos2)
	local xDif = math.abs(pos1.x - pos2.x)
	local yDif = math.abs(pos1.y - pos2.y)

	local posDif = math.max(xDif, yDif)
	if (pos1.z ~= pos2.z) then
		posDif = (posDif + 9 + 6)
	end
	return posDif
end

function getPlayerLookPos(cid)
	return getPosByDir(getThingPos(cid), getPlayerLookDir(cid))
end

function getPosByDir(basePos, dir)
	local pos = basePos
	if(dir == NORTH) then
		pos.y = pos.y-1
	elseif(dir == SOUTH) then
		pos.y = pos.y + 1
	elseif(dir == WEST) then
		pos.x = pos.x-1
	elseif(dir == EAST) then
		pos.x = pos.x+1
	elseif(dir == NORTHWEST) then
		pos.y = pos.y-1
		pos.x = pos.x-1
	elseif(dir == NORTHEAST) then
		pos.y = pos.y-1
		pos.x = pos.x+1
	elseif(dir == SOUTHWEST) then
		pos.y = pos.y+1
		pos.x = pos.x-1
	elseif(dir == SOUTHEAST) then
		pos.y = pos.y+1
		pos.x = pos.x+1
	end
	return pos
end

function playerExists(name)
	return (getPlayerGUIDByName(name) ~= 0)
end

function getConfigInfo(info)
	if (type(info) ~= 'string') then return nil end

	dofile('config.lua')
	return _G[info]
end

exhaustion = 
{

	check = function (cid, storage)
		local exhaust = getPlayerStorageValue(cid, storage)  
		if (os.time() >= exhaust) then
			return FALSE
		else
			return TRUE
		end
	end,

	get = function (cid, storage)
		local exhaust = getPlayerStorageValue(cid, storage) 
		local left = exhaust - os.time()
		if (left >= 0) then
			return left
		else
			return FALSE
		end
	end,
	
	set = function (cid, storage, time)
		setPlayerStorageValue(cid, storage, os.time()+time)  
	end,

	make = function (cid, storage, time)
		local exhaust = exhaustion.get(cid, storage)
		if (exhaust > 0) then
			return FALSE
		else
			exhaustion.set(cid, storage, time)
			return TRUE
		end
	end
}


	table.find = function (table, value)
		for i,v in pairs(table) do
			if (v == value) then
				return i
			end
		end
		return nil
	end
	table.getPos = table.find

	table.isStrIn = function (txt, str)
		local result = false
		for i, v in pairs(str) do          
			result = (string.find(txt, v) and not string.find(txt, '(%w+)' .. v) and not string.find(txt, v .. '(%w+)'))
			if (result) then
				break
			end
		end
		return result
	end

	table.countElements = function (table, item)
		local count = 0
		for i, n in pairs(table) do
			if (item == n) then count = count + 1 end
		end
		return count
	end
	
	table.getCombinations = function (table, num)
		local a, number, select, newlist = {}, #table, num, {}
		for i = 1, select do
			a[#a + 1] = i
		end
		local newthing = {}
		while (1) do
			local newrow = {}
			for i = 1, select do
				newrow[#newrow + 1] = table[a[i]]
			end
			newlist[#newlist + 1] = newrow
			i = select
			while (a[i] == (number - select + i)) do
				i = i - 1
			end
			if (i < 1) then break end
				a[i] = a[i] + 1
				for j = i, select do
					a[j] = a[i] + j - i
				end
			end
		return newlist
	end


	string.split = function (str)
		local t = {}
		local function helper(word) table.insert(t, word) return "" end
		if (not str:gsub("%w+", helper):find"%S") then return t end
	end
	
	string.separate = function(separator, string)
		local a, b = {}, 0
		if (#string == 1) then return string end
	    while (true) do
			local nextSeparator = string.find(string, separator, b + 1, true)
			if (nextSeparator ~= nil) then
				table.insert(a, string.sub(string,b,nextSeparator-1)) 
				b = nextSeparator + 1 
			else
				table.insert(a, string.sub(string, b))
				break 
			end
	    end
		return a
	end

function isSorcerer(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc == 1 or voc == 5) then
			return TRUE
		end
	end
	
	return FALSE
end

function isDruid(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc == 2 or voc == 6) then
			return TRUE
		end
	end
	
	return FALSE
end

function isPaladin(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc == 3 or voc == 7) then
			return TRUE
		end
	end
	
	return FALSE
end

function isKnight(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc == 4 or voc == 8) then
			return TRUE
		end
	end
	
	return FALSE
end

function isPromoted(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc > 4 and voc < 8) then
			return TRUE
		end
	end
	
	return FALSE
end

function isNotPromoted(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc > 1 and voc < 5) then
			return TRUE
		end
	end
	
	return FALSE
end

function isGameMaster(cid)
	if (isPlayer(cid) == TRUE) then
		local gm = getPlayerAccess(cid)
		if (gm > 0) then
			return TRUE
		end
	end
	
	return FALSE
end

function isPlayerFromRook(cid)
	if (isPlayer(cid) == TRUE) then
		local voc = getPlayerVocation(cid)
		if (voc == 0) then
			return TRUE
		end
	end
	
	return FALSE
end

function isNumber(str)
	return tonumber(str) ~= nil and TRUE or FALSE
end

function decayItem(params)
	local item = getThingfromPos(params.itempos)
	local toitem = params.toitem
	if toitem == nil or toitem == 0 then
		doRemoveItem(item.uid,1)
	else
		doTransformItem(item.uid,toitem)
	end
end
 
function doDecayItemTo(itempos, toitem, timer)
	params = {itempos=itempos, toitem=toitem}
	addEvent(decayItem, timer*1000, params)
end

function getNumber(txt)
	x = string.gsub(txt,"%a","")
	x = tonumber(x)
	if x ~= nill and x > 0 then
		return x
	else
		return 0
	end
end

function getItemWeightByID(itemid)
   local uid = doCreateItemEx(itemid)
   local ret = getItemWeight(uid)
   doRemoveItem(uid)
   return (ret)
end

function percentChance(chance)
    if chance >= math.random(1, 100) then
        return true
    else
        return false
    end
end

function CheckPlayerPromotion(cid)
	-- Promote player if needed
	if isPremium(cid) == TRUE and getPlayerStorageValue(cid, 30005) == 1 and isNotPromoted(cid) == TRUE then
		doPlayerSetVocation(cid, getPlayerVocation(cid)+4)
		setPlayerStorageValue(cid, 30005, -1)
	end
	-- Remover Promotion
	if isPromoted(cid) == TRUE and isPremium(cid) == FALSE then
		doPlayerSetVocation(cid, getPlayerVocation(cid)-4)
		setPlayerStorageValue(cid, 30005, 1)
	end
	return true
end

function getTibiaTime()
	local worldTime = getWorldTime()
	local hours = 0
	while (worldTime > 60) do
		hours = hours + 1
		worldTime = worldTime - 60
	end

	return tostring(hours .. ':' .. worldTime)
end

function doPlayerBuyContainer(cid, container, itemid, charges, cost, count)
	if doPlayerRemoveMoney(cid, cost) == TRUE then
		for i = 1, count do
			local backpack = doPlayerAddItem(cid, container, 1)
			for x = 1, 20 do
				doAddContainerItem(backpack, itemid, charges)
			end
		end
		return LUA_NO_ERROR
	end
	return LUA_ERROR
end

function doPlayerBuyItemContainer(cid, container, itemid, charges, cost, count)
	if getPlayerFreeCap(cid) > (getItemWeightById(itemid, 20)*count + getItemWeightById(container, count)) then
		if doPlayerRemoveMoney(cid, cost) == TRUE then
			for i = 1, count do
				local containerItem = doPlayerAddItem(cid, container, 1)
				for x = 1, 20 do
					doAddContainerItem(containerItem, itemid, charges)
				end
			end
			return LUA_NO_ERROR
		end
	else
		return
	end
	return LUA_ERROR
end

function setPlayerQuest(cid, local_item, storageid, itemid, count)
    if getPlayerStorageValue(cid, storageid) == -1 then
        if getPlayerFreeCap(cid) >= getItemWeightByID(itemid, 1) then
            doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, 'You have found a ' .. getItemName(itemid) .. '.')
            doPlayerAddItemEx(cid, itemid, count)
            setPlayerStorageValue(cid, storageid, 1)
        else
            doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, 'You have found a ' .. getItemName(itemid) .. ' weighing ' .. getItemWeightByID(itemid, 1) .. ' oz. It\'s too heavy.')
        end
    else
        doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "The ".. local_item .." is empty.")
    end
return TRUE
end

function getItemNameById(itemid)
   return getItemDescriptions(itemid).name
end