

function onUse(cid, item, frompos, item2, topos)

	if item.actionid == 0 then
		doTransformItem(item.uid,item.itemid+1)
	else
		doPlayerSendTextMessage(cid,22,"Sorry, not possible.")
	end
	return 1

end