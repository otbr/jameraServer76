local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_HEALING)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_BLUE)
setCombatParam(combat, COMBAT_PARAM_TARGETCASTERORTOPMOST, 1)
setCombatParam(combat, COMBAT_PARAM_AGGRESSIVE, 0)
setCombatParam(combat, COMBAT_PARAM_DISPEL, CONDITION_PARALYZE)

function onGetFormulaValues(cid, level, maglevel)
if getPlayerVocation(cid) == 2 or getPlayerVocation(cid) == 4 then
	min = (level * 0.4 + maglevel * 0.9) * 1.9 + 11
	max = (level * 0.7 + maglevel * 1.1) * 2.1 + 33
else
	min = (level * 0.4 + maglevel * 0.6) * 1.9 + 11
	max = (level * 0.6 + maglevel * 0.9) * 2.1 + 33
end
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end
