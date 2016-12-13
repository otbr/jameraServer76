function onLogin(cid)
	registerCreatureEvent(cid, "PlayerDeath")
	registerCreatureEvent(cid, "VipCheck")

     
end


function onLogin(cid)
	if getPlayerPremiumDays(cid) == 0 then
		setPlayerStorageValue(cid,30009,-1)
	end
	return TRUE
end




