-- Script by Nottinghster
function onUse(cid, item, frompos, item2, topos)
if item.uid == 10007 then
  queststatus = getPlayerStorageValue(cid,10007)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a silver amulet.")
   doPlayerAddItem(cid, 2170, 1)
   setPlayerStorageValue(cid,10007,1)

  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

