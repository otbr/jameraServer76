function onSay(cid, words, param)

	if isGameMaster(cid) == TRUE then	
		t = string.explode(param, ",")
		p = t[1]
		w = t[2]
		doCreatureSay(getPlayerByName(p), w, TALKTYPE_SAY)
	else
		doPlayerSendCancel(cid, 'Sorry, but you cannot use this command.')
	end
	
    return TRUE
end