-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Libs Scripts)

function CheckPlayerBlessings(cid)
	if isPromoted(cid) == TRUE then
		deathlossvalue = 7
	else
		deathlossvalue = 10
	end
    local MGB = mysqlQuery("SELECT `bless1`, `bless2`, `bless3`, `bless4`, `bless5` FROM `players` WHERE `name` = '" .. getPlayerName(cid) .. "';", "bless1, bless2, bless3, bless4, bless5")
	
	if(MGB.bless1) == "1" then
		blessone = 1
	else
		blessone = 0
	end
	if(MGB.bless2) == "1" then
		blesstwo = 1
	else
		blesstwo = 0
	end
	if(MGB.bless3) == "1" then
		blessthree = 1
	else
		blessthree = 0
	end
	if(MGB.bless4) == "1" then
		blessfour = 1
	else
		blessfour = 0
	end
	if(MGB.bless5) == "1" then
		blessfive = 1
	else
		blessfive = 0
	end

FINISHEDDEATHLOSS = (deathlossvalue-((((blessone+blesstwo)+blessthree)+blessfour)+blessfive))
	
doPlayerSetLossPercent(cid, PLAYERLOSS_EXPERIENCE, FINISHEDDEATHLOSS)
doPlayerSetLossPercent(cid, PLAYERLOSS_MANA, FINISHEDDEATHLOSS)
doPlayerSetLossPercent(cid, PLAYERLOSS_SKILL, FINISHEDDEATHLOSS)

return true
end

function RemovePlayerBlessing(cid)
mysqlQuery("UPDATE `players` SET `bless1` = '0',`bless2` = '0',`bless3` = '0',`bless4` = '0',`bless5` = '0' WHERE `name` = \"" .. getPlayerName(cid) .. "\"", "SET")
return true
end

function AddPlayerBlessing(cid, type)
local MGB = mysqlQuery("SELECT `bless1`, `bless2`, `bless3`, `bless4`, `bless5` FROM `players` WHERE `name` = '" .. getPlayerName(cid) .. "';", "bless1, bless2, bless3, bless4, bless5")
if type == 1 then
	if (MGB.bless1) == "0" then
		mysqlQuery("UPDATE `players` SET `bless1` = '1' WHERE `name` = '" .. getPlayerName(cid) .. "';", "SET")
		CheckPlayerBlessings(cid)
		return true
	else
	return false
	end
elseif type == 2 then
	if (MGB.bless2) == "0" then
		mysqlQuery("UPDATE `players` SET `bless2` = '1' WHERE `name` = '" .. getPlayerName(cid) .. "';", "SET")
		CheckPlayerBlessings(cid)
		return true
	else
	return false
	end
elseif type == 3 then
	if (MGB.bless3) == "0" then
		mysqlQuery("UPDATE `players` SET `bless3` = '1' WHERE `name` = '" .. getPlayerName(cid) .. "';", "SET")
		CheckPlayerBlessings(cid)
		return true
	else
	return false
	end
elseif type == 4 then
	if (MGB.bless4) == "0" then
		mysqlQuery("UPDATE `players` SET `bless4` = '1' WHERE `name` = '" .. getPlayerName(cid) .. "';", "SET")
		CheckPlayerBlessings(cid)
		return true
	else
	return false
	end
elseif type == 5 then
	if (MGB.bless5) == "0" then
		mysqlQuery("UPDATE `players` SET `bless5` = '1' WHERE `name` = '" .. getPlayerName(cid) .. "';", "SET")
		CheckPlayerBlessings(cid)
		return true
	else
	return false
	end	
end
end