function onUse(cid, item, frompos, item2, topos)
if item.uid == 4500 then
  queststatus = getPlayerStorageValue(cid,4500)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a blood herb.")
   item_uid = doPlayerAddItem(cid,2798,1)
   setPlayerStorageValue(cid,4500,1)

  else
   doPlayerSendTextMessage(cid,22,"The dead tree is empty.")
  end
else
  return 0
end
return 1
end

