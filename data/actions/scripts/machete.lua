-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Action)

function onUse(cid, item, frompos, item2, topos)
	if (isInArray(JUNGLE_GRASS_REMOVE, item2.itemid) == TRUE) then
		doRemoveItem(item2.uid)
		return TRUE
		
	elseif (isInArray(JUNGLE_GRASS_TRANSFORM, item2.itemid) == TRUE) then
		doTransformItem(item2.uid, item2.itemid - 1)
		doDecayItem(item2.uid)
		return TRUE
		
	elseif (isInArray(BAMBOO_WALL, item2.itemid) == TRUE) then
		doRemoveItem(item2.uid)
		return TRUE	
		
	end
	
	return FALSE
end 