-- Script by Nottinghster
function onUse(cid, item, frompos, item2, topos)
if item.uid == 1015 then
  queststatus = getPlayerStorageValue(cid,1015)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a triple ultimate healing rune.")
   doPlayerAddItem(cid, 2273, 3)
   setPlayerStorageValue(cid,1015,1)

  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end

