function onUse(cid, item, frompos, item2, topos)
if item.uid == 8016 then
  queststatus = getPlayerStorageValue(cid,8016)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a war hammer.")
   item_uid = doPlayerAddItem(cid,2391,1)
   setPlayerStorageValue(cid,8016,1)
  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

