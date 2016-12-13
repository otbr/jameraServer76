local light = createConditionObject(CONDITION_LIGHT)
setConditionParam(light, CONDITION_PARAM_LIGHT_LEVEL, 10)
setConditionParam(light, CONDITION_PARAM_LIGHT_COLOR, 215)
setConditionParam(light, CONDITION_PARAM_TICKS, (33*60)*1000) --33 minutes(time in ms)

function onSay(cid, words, param)
	if isGameMaster(cid) == FALSE then 
       doPlayerSendTextMessage(cid, MESSAGE_STATUS_DEFAULT, "Only gamemasters or higher can execute this command.") 
	else
		doAddCondition(cid, light)
		doSendMagicEffect(getCreaturePosition(cid), CONST_ME_MAGIC_BLUE)
	end
	
	return TRUE
end