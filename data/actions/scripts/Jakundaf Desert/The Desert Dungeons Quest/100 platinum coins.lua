function onUse(cid, item, frompos, item2, topos)
if item.uid == 10103 then
  queststatus = getPlayerStorageValue(cid,10103)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a 100 platinum coins.")
   doPlayerAddItem(cid, 2152, 100)
   setPlayerStorageValue(cid,10103,1)

  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end