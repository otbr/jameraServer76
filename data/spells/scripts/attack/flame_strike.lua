local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_FIREDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_FIREAREA)

function onGetFormulaValues(cid, level, maglevel)
if getPlayerVocation(cid) == 1 or getPlayerVocation(cid) == 5 then
	min = -(level * 0.6 + maglevel * 0.8) * 1.9
	max = -(level * 0.7 + maglevel * 0.9) * 2.1
else
	min = -(level * 0.6 + maglevel * 0.7) * 1.9
	max = -(level * 0.7 + maglevel * 0.8) * 2.1
end	
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local arr = {
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 0, 0},
{0, 3, 0}
}

local area = createCombatArea(arr)
setCombatArea(combat, area)

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end