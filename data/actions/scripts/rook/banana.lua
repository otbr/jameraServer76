function onUse(cid, item, frompos, item2, topos)
if item.uid == 3032 then
  queststatus = getPlayerStorageValue(cid,3032)
  if queststatus == -1 or queststatus == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a banana.")
   item_uid = doPlayerAddItem(cid,2676,1)
   setPlayerStorageValue(cid,3032,1)

  else
   doPlayerSendTextMessage(cid,22,"it\'s empty.")
  end
else
  return 0
end
return 1
end

