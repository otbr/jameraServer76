

function onUse(cid, item, frompos, item2, topos)

	if item.actionid == 1221 then
		doTransformItem(item.uid,item.itemid+1)
	else
		doPlayerSendTextMessage(cid,22,"It is locked.")
	end
	return 1

end