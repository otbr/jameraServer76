function onStepIn(cid, item, pos)
 
	exit1 = {x=32566, y=31963, z=1, stackpos=255}
 
	if item.actionid == 8015 then
 
		doTeleportThing(cid,exit1)
 
	end
	return TRUE
end