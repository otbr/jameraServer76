function onLogin(cid)
	--registerCreatureEvent(cid, "PlayerDeath")
	--registerCreatureEvent(cid, "VipCheck")
	registerCreatureEvent(cid, "onPlayerKill")
        --registerCreatureEvent(cid, "TaskLook")
        if getPlayerPremiumDays(cid) == 0 then
                setPlayerStorageValue(cid,30009,-1)
        end
        return TRUE    
end






