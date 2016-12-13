-- Script by Rodrigo (Nottinghster)
function onSay(cid, words, param)
    if getPlayerPremiumDays(cid) <= 365 then
	
	--[[if isPlayerVip(cid) == TRUE then
	doPlayerSendCancel(cid, "You can not buy Premium Account if you donate.")
    return TRUE
    end]]--
	
        local priceDay = 1000
        if param == "" then
            cost = priceDay
            oneDay = 1
            if doPlayerRemoveMoney(cid, cost) == TRUE then
                doPlayerAddPremiumDays(cid, oneDay)
                doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "You have bought one day of premium account. Please login again!")
            else
                doPlayerSendCancel(cid, "You don't have enough money.")
                doSendMagicEffect(getPlayerPosition(cid), CONST_ME_POFF)
            end
        else
            if getPlayerPremiumDays(cid) + param <= 365 then
            local cost = param * priceDay
            if doPlayerRemoveMoney(cid, cost) == TRUE then
                doPlayerAddPremiumDays(cid, param)
                doPlayerSendTextMessage(cid, MESSAGE_INFO_DESCR, "You have bought ".. param .." days of premium account. Please login again!")
            else
                doPlayerSendCancel(cid, "You don't have enough money.")
                doSendMagicEffect(getPlayerPosition(cid), CONST_ME_POFF)
            end
            else
                doPlayerSendCancel(cid, "You can not buy more than one year of Premium Account.")
                doSendMagicEffect(getPlayerPosition(cid), CONST_ME_POFF)
            end
        end
            
    else
        doPlayerSendCancel(cid, "You can not buy more than one year of Premium Account.")
        doSendMagicEffect(getPlayerPosition(cid), CONST_ME_POFF)
    end
end