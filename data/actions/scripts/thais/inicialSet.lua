function onUse(cid, item, frompos, item2, topos)
if item.uid == 3038 then
  queststatus = getPlayerStorageValue(cid,3038)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a bag.")
   bag = doPlayerAddItem(cid,1987,1)
   doAddContainerItem(bag,2143,1)
   doAddContainerItem(bag,2144,1)
   doAddContainerItem(bag,2148,100)
   setPlayerStorageValue(cid,3038,1)
  else
   doPlayerSendTextMessage(cid,22,"The box is empty.")
  end
else
  return 0
end
return 1
end

