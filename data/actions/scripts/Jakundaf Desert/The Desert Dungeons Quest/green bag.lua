function onUse(cid, item, frompos, item2, topos)
if item.uid == 10104 then
  queststatus = getPlayerStorageValue(cid,10104)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a green bag.")
   bag = doPlayerAddItem(cid, 1991, 1)
   doAddContainerItem(bag, 2200, 1)
   doAddContainerItem(bag, 2214, 1)
   doAddContainerItem(bag, 2162, 1)
   doAddContainerItem(bag, 2193, 1)
   setPlayerStorageValue(cid,10104,1)

  else
   doPlayerSendTextMessage(cid,22,"The chest is empty.")
  end
else
  return 0
end
return 1
end