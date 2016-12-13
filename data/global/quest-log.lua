dofile(getDataDir() .. 'talkactions/scripts/test.lua')
-----------MAIN QUETS-----------
ql1 = "#Sams Old Backpack: ".. quest1 .."."
ql02 = ""
ql03 = ""
ql04 = ""
ql05 = ""
ql06 = ""
ql07 = ""
ql08 = ""
ql09 = ""
ql10 = ""
ql11 = ""
ql12 = ""
ql13 = ""
ql14 = ""
ql15 = ""
ql16 = ""
ql17 = ""



-----------ROOK QUESTS-----------












-----------SYSTEM-----------
if getPlayerStorageValue(cid, 8289) == 0 then
quest1 = "[1/1]"
else
quest1 = "[0/1]"
end

if getPlayerStorageValue(cid, 666) >= 1 then
quest2 = "step 3/7 done"
else
quest2 = "[0/1]"
end

if getPlayerStorageValue(cid, 1666) >= 1 then
quest3 = "step 3/7 done"
else
quest3 = "[3/7]"
end