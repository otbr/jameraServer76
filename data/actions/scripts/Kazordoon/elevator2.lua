local t = {
{x=32636, y=31881, z=7},
{x=32636, y=31881, z=2}
}

function onUse(cid, item, fromPosition, itemEx, toPosition)
local r = item.itemid == 1945
doRelocate(t[r and 1 or 2], t[r and 2 or 1])
doSendMagicEffect(t[1], CONST_ME_POFF)
doSendMagicEffect(t[2], CONST_ME_POFF)
return doTransformItem(item.uid, r and 1946 or 1945)
end