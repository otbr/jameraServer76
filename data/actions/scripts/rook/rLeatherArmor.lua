function onUse(cid, item, frompos, item2, topos)
if item.uid == 3037 then
  queststatus = getPlayerStorageValue(cid,3037)
  if queststatus == -1 or queststatus == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a reinforced leather armor.")
   item_uid = doPlayerAddItem(cid,5178,1)
   setPlayerStorageValue(cid,3037,1)

  else
   doPlayerSendTextMessage(cid,22,"it\'s empty.")
  end
else
  return 0
end
return 1
end

