function onUse(cid, item, frompos, item2, topos)
if item.uid == 4010 then
  queststatus = getPlayerStorageValue(cid,4010)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a naginata.")
   item_uid = doPlayerAddItem(cid,2426,1)
   setPlayerStorageValue(cid,4010,1)
  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

