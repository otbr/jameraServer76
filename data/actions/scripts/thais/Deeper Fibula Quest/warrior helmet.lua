function onUse(cid, item, frompos, item2, topos)
if item.uid == 4005 then
  queststatus = getPlayerStorageValue(cid,4005)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a warrior helmet.")
   item_uid = doPlayerAddItem(cid,2475,1)
   setPlayerStorageValue(cid,4005,1)

  else
   doPlayerSendTextMessage(cid,22,"The dead human is empty.")
  end
else
  return 0
end
return 1
end

