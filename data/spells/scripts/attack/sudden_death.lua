local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_MORTAREA)
setCombatParam(combat, COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_SUDDENDEATH)

function onGetFormulaValues(cid, level, maglevel)
if getPlayerVocation(cid) == 1 or getPlayerVocation(cid) == 5 or getPlayerVocation(cid) == 2 or getPlayerVocation(cid) == 6 and getPlayerLevel(cid) <= 99 then
min = -(level * 2 + maglevel *3) * 1.450
max = -(level * 2 + maglevel *3) * 1.747
elseif getPlayerVocation(cid) == 1 or getPlayerVocation(cid) == 5 or getPlayerVocation(cid) == 2 or getPlayerVocation(cid) == 6 and getPlayerLevel(cid) >= 100 then
min = -(level * 2 + maglevel *3) * 1.250
max = -(level * 2 + maglevel *3) * 1.547
else
min = -(level * 2 + maglevel *3) * 1.450
max = -(level * 2 + maglevel *3) * 1.747
end
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end
