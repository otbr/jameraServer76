function onUse(cid, item, frompos, item2, topos)
	if item.uid==4015 then
		newpos = {x=32172, y=32439, z=8}
		doTeleportThing(cid,newpos)
		return 1
	end
end