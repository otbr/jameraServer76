-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Libs Scripts)

local vip_storage = 30010
 
function setPlayerVipDays(cid, days)
    return setPlayerStorageValue(cid, vip_storage, (os.time() + 86400 * days)) and days
end
 
function getPlayerVipDays(cid)
    local left, d = (getPlayerStorageValue(cid, vip_storage) - os.time()), 0
		while left >= 86400 do
			left = left - 86400
			d = d+1
		end
    return d
end
 
function isPlayerVip(cid)
    return getPlayerStorageValue(cid, vip_storage) >= os.time() and TRUE or FALSE
end
 
function doPlayerAddVipDays(cid, days)
    return setPlayerStorageValue(cid, vip_storage, (os.time() + (86400 * getPlayerVipDays(cid)) + (86400 * days))) and days
end
 
function doPlayerRemoveVip(cid)
    return setPlayerStorageValue(cid, vip_storage, 0) and TRUE
end