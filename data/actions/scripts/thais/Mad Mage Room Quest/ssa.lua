function onUse(cid, item, frompos, item2, topos)
if item.uid == 4008 then
  queststatus = getPlayerStorageValue(cid,4008)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a stone skin amulet.")
   doPlayerAddItem(cid,2197,5)
   setPlayerStorageValue(cid,4008,1)
  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

