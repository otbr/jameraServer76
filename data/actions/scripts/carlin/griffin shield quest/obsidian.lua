function onUse(cid, item, frompos, item2, topos)
if item.uid == 9000 then
  queststatus = getPlayerStorageValue(cid,9000)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a obsidian lance.")
   doPlayerAddItem(cid,2425,1)
   setPlayerStorageValue(cid,9000,1)
  else
   doPlayerSendTextMessage(cid,22,"The dead human is empty.")
  end
else
  return 0
end
return 1
end

