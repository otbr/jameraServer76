function onUse(cid, item, frompos, item2, topos)
if item.uid == 3025 then
  requiredCap = 102 -- look how much cap have this bag and replace CAP
  queststatus = getPlayerStorageValue(cid, 3025)
  if queststatus == -1 or queststatus == 0 then
    if getPlayerFreeCap(cid) >= requiredCap then
      doPlayerSendTextMessage(cid,22,"You have found a bag.")
      container = doPlayerAddItem(cid, 1987, 1)
      doAddContainerItem(container,2530,1)
      doAddContainerItem(container,2480,1)
      setPlayerStorageValue(cid,3025,1)
    else
      doPlayerSendTextMessage(cid,22,"You don't have enough cap.")
    end
  else
   doPlayerSendTextMessage(cid,22,"it\'s empty.")
  end
end
return 1
end