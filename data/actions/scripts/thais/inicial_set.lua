function onUse(cid, item, frompos, item2, topos)
if item.uid == 4019 then
  queststatus = getPlayerStorageValue(cid,4019)
  if queststatus == -1 and getPlayerAccess(cid) == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a bag.")
   bag = doPlayerAddItem(cid,1988,1)
	doAddContainerItem(bag,5180,1)
	doAddContainerItem(bag,5181,1)
	doAddContainerItem(bag,5178,1)
	doAddContainerItem(bag,5179,1)
	doAddContainerItem(bag,5158,1)
	doAddContainerItem(bag,2394,1)
	doAddContainerItem(bag,2423,1)
	doAddContainerItem(bag,2383,1)
   setPlayerStorageValue(cid,4019,1)
  else
   doPlayerSendTextMessage(cid,22,"The box is empty.")
  end
else
  return 0
end
return 1
end



