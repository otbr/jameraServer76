local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_EXPLOSIONAREA)

function onGetFormulaValues(cid, level, maglevel)

if getPlayerLevel(cid) <= 99 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.6 * 1.2
	max = -(level * 1.3 + maglevel * 5.4) * 1.9 * 1.2
elseif getPlayerLevel(cid) >= 100 and getPlayerLevel(cid) <= 129 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.5 * 1.2
	max = -(level * 1.3 + maglevel * 5.4) * 1.8 * 1.2
elseif getPlayerLevel(cid) >= 130 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.4 * 1.2
	max = -(level * 1.3 + maglevel * 5.4) * 1.7 * 1.2
else
	min = -(level * 1.1 + maglevel * 4.1) * 1.4 * 1.2
	max = -(level * 1.3 + maglevel * 5.4) * 1.7 * 1.2
end
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

arr = {
{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
{0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
{1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1},
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
{0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
}

local area = createCombatArea(arr)

setCombatArea(combat, area)

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end