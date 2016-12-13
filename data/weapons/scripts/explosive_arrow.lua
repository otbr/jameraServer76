local combat = createCombatObject()
setCombatParam(combat, COMBAT_PARAM_TYPE, COMBAT_PHYSICALDAMAGE)
setCombatParam(combat, COMBAT_PARAM_EFFECT, CONST_ME_FIREAREA)
setCombatParam(combat, COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_BURSTARROW)
setCombatFormula(combat, COMBAT_FORMULA_LEVELMAGIC, 0, 0, -.55, 0) -- Original
--setCombatFormula(combat, COMBAT_FORMULA_LEVELMAGIC, -0.35, -30, -0.45, -30) -- by Kadall
--setCombatFormula(combat, COMBAT_FORMULA_SKILL, 1, 0, 1, 0) -- TFS 0.4

local area = createCombatArea( { {1, 1, 1}, {1, 3, 1}, {1, 1, 1} } )
setCombatArea(combat, area)

function onUseWeapon(cid, var)
	return doCombat(cid, combat, var)
end
