-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Creature Scripts)

function onAdvance(cid)

	-- Trial Premium
	if getPlayerStorageValue(cid, 30004) == -1 and getPlayerLevel(cid) == 2 and getPlayerPremiumDays(cid) == 0 and getGlobalStorageValue(getPlayerAccountId(cid)) == -1 then
		doPlayerAddPremiumDays(cid, 3)
		doPlayerSendTextMessage(cid, MESSAGE_EVENT_ADVANCE, "You have received 3 days of trial premium account, enjoy it!")
		setPlayerStorageValue(cid, 30004, 1)
		setGlobalStorageValue(getPlayerAccountId(cid),1)
	end

	return TRUE
end