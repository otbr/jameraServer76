function onUse(cid, item, frompos, item2, topos)
if item.uid == 9503 then
  queststatus = getPlayerStorageValue(cid,9503)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a morning star.")
   doPlayerAddItem(cid,2394,1)
   setPlayerStorageValue(cid,9503,1)
  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

