function onSay(cid, words, param, target)
    if isGameMaster(cid) == FALSE then
        doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "You can not use this command.") 
    elseif param ~= "" then 
        if string.find(param, ',') ~= nil then 
            local sep1 = string.find(param, ',') 
            local param1 = string.sub(param, 1, sep1-1) 
            local param2 = string.sub(param,sep1+1,string.len(param)) 
            local target = getPlayerByName(param1)
            if target == FALSE then 
                doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "A player with that name is not online") 
            elseif param1 == "" then 
                doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "You must type a player.") 
            elseif (param2 == "") then 
                doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "You must type amount of days.") 
            elseif isNumber(param2) == FALSE then 
                doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "Days have to be a number.") 
            elseif words == "!addpremium" then 
                local player = getPlayerName(target)
                local days = param2 
                if days ~= 0 then 
                    doPlayerAddPremiumDays(target, days) 
                    doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "You have added "..days.." day(s) of premium to "..player..".") 
                    doPlayerSendTextMessage(target, MESSAGE_STATUS_DEFAULT, "You have received "..days.." days of premium account from GOD Nottinghster.")
                else 
                    doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "Days have to be higher than 0.") 
                end 
            end 
        end 
    end 
    return TRUE 
end  