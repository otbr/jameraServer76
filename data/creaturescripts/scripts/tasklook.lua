function onLook(cid, thing, position, lookDistance)
	if isPlayer(thing.uid) and getTaskPoints(thing.uid) > 0 then
		doPlayerSetSpecialDescription(thing.uid, "\n"..(getPlayerSex(thing.uid) == 0 and "She" or "He").. " is a "..GetRankTask(thing.uid))
	end	
	return true
end
