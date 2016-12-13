local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_ENERGYDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_ENERGYAREA)
setCombatParam(combat, COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_ENERGY)

function onGetFormulaValues(cid, level, maglevel)
if getPlayerLevel(cid) <= 99 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.5
	max = -(level * 1.3 + maglevel * 5.4) * 1.8
elseif getPlayerLevel(cid) >= 100 and getPlayerLevel(cid) <= 129 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.4
	max = -(level * 1.3 + maglevel * 5.4) * 1.7
elseif getPlayerLevel(cid) >= 130 then
	min = -(level * 1.1 + maglevel * 4.1) * 1.3
	max = -(level * 1.3 + maglevel * 5.4) * 1.6
else
	min = -(level * 1.1 + maglevel * 4.1) * 1.3
	max = -(level * 1.3 + maglevel * 5.4) * 1.6
end
	return min, max
end

setCombatCallback(combat, CALLBACK_PARAM_LEVELMAGICVALUE, "onGetFormulaValues")

local arr = {
{1, 1, 1},
{1, 1, 1},
{1, 1, 1},
{0, 1, 0},
{0, 3, 0},
}

local arrDiag = {
{1, 1, 1, 0, 0},
{1, 1, 0, 0, 0},
{1, 0, 1, 0, 0},
{0, 0, 0, 1, 0},
{0, 0, 0, 0, 3},
}

local area = createCombatArea(arr, arrDiag)

setCombatArea(combat, area)

function onCastSpell(cid, var)
	return doCombat(cid, combat, var)
end