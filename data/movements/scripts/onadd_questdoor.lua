function onAddItem(item, tile, pos)
	if(item.actionid == 0) then
		-- It's a normal door, we don't need special descriptions.
		return TRUE
	end

	doSetItemSpecialDescription(item.uid, "The door is sealed against unwanted intruders.")
end