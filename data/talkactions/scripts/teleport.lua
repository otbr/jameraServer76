 function onSay(cid, words, param)
local temple = { x = 32369, y = 32241, z = 7 } 
	if getPlayerAccess(cid) < 4 then
        doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "You can not use this command.") 
	return TRUE
	end
        for _, cid in ipairs(getPlayersOnlineList()) do
		if isGameMaster(cid) == FALSE and getPlayerVocation(cid) > 0 then	
                if(param == '') then
				doTeleportThing(cid, temple)
				doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "All players have been teleported to Thais.")
				doSendMagicEffect(getCreaturePosition(cid), CONST_ME_MAGIC_BLUE)
			else
				local t = string.explode(param, ",")
				doTeleportThing(cid, { x = t[1], y = t[2], z = t[3] } )
				doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "All players have been teleported to the coords [X:"..t[1].."], [Y:"..t[2].."], [Z:"..t[3].."]")
				doSendMagicEffect(getCreaturePosition(cid), CONST_ME_MAGIC_BLUE)
			end
        end
    end	
end