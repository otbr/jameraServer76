local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_HEALING)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_BLUE)
setCombatParam(combat, COMBAT_PARAM_TARGETCASTERORTOPMOST, 1)
setCombatParam(combat, COMBAT_PARAM_AGGRESSIVE, 0)
setCombatParam(combat, COMBAT_PARAM_DISPEL, CONDITION_PARALYZE)

function onGetFormulaValues(cid, level, maglevel)
if getPlayerVocation(cid) == 1 or getPlayerVocation(cid) == 5 then
	min = (level * 0.2 + maglevel * 0.4) * 1.9 + 20
	max = (level * 0.3 + maglevel * 0.5) * 2.1 + 30
elseif getPlayerVocation(cid) == 2 or getPlayerVocation(cid) == 4 then
	min = (level * 0.2 + maglevel * 0.4) * 2.1 + 20
	max = (level * 0.3 + maglevel * 0.5) * 2.9 + 30
else
	min = (level * 0.2 + maglevel * 0.4) * 1.9 + 20
	max = (level * 0.3 + maglevel * 0.5) * 2.1 + 30
end
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end