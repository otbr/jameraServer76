function onUse(cid, item, frompos, item2, topos)
	local count = getPlayerInstantSpellCount(cid)
	local text = ""
	local t = {}

	for i=0, count-1 do
		local spell = getPlayerInstantSpellInfo(cid, i)
		if spell.mlevel <= 0 or spell.mlevel >= 0 then
			if spell.manapercent > 0 then
				spell.mana = spell.manapercent .. "%"
			end

			table.insert(t, spell)
		end
	end
	
	table.sort(t, function(a, b) return a.mlevel < b.mlevel end)

	local prevLevel = -1
	for i,spell in ipairs(t) do
		local line = ""
		if prevLevel ~= spell.mlevel then
			if i ~= 1 then
				line = "\n"
			end

			line = line .. "Spells for magic level " .. spell.mlevel .. "\n"
			prevLevel = spell.mlevel
		end
		
		text = text .. line .. "  " .. spell.words .. " - " .. spell.name .. " : " .. spell.mana .. "\n"
	end
		
	doShowTextDialog(cid, item.itemid, text)
	return 1
end
