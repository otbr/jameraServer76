function onUse(cid, item, frompos, item2, topos)
if item.uid == 8014 then
  queststatus = getPlayerStorageValue(cid,8014)
  if queststatus == -1 or queststatus == 0 then
   doPlayerSendTextMessage(cid,22,"You have found a tempest rod.")
   item_uid = doPlayerAddItem(cid,2183,1)
   setPlayerStorageValue(cid,8014,1)

  else
   doPlayerSendTextMessage(cid,22,"it\'s empty.")
  end
else
  return 0
end
return 1
end

