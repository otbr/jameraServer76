-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Movement Scripts)

function onAddItem(item, tile, pos)
	local isLevelDoor = (item.actionid >= 1001 and item.actionid <= 1999)
	local isVocationDoor = (item.actionid >= 2001 and item.actionid <= 2008)

	if not(isLevelDoor or isVocationDoor) then
		return TRUE
	end

	if(isLevelDoor) then
		local level = item.actionid - 1000
		doSetItemSpecialDescription(item.uid, "It is a gate for level " .. level .. ".\n Only the worthy may pass.")
	else
		local vocDescriptions = {
			[1] = "sorcerers", [2] = "druids", [3] = "paladins", [4] = "knights",
			[5] = "master sorcerers", [6] = "elder druids", [7] = "royal paladins", [8] = "elite knights"
		}
		local voc = vocDescriptions[item.actionid-2000]
		doSetItemSpecialDescription(item.uid, "It is a gate for " .. voc .. ".\n Only the worthy may pass.")
	end
	return TRUE
end