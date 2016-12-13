-- Author: 		Rodrigo (Nottinghster) - (OTLand, OTFans, XTibia, OTServBR)
-- Country:		Brazil
-- From: 		Tibia World RPG OldSchool
-- Email: 		god.rodrigo@hotmail.com
-- Compiler:	Tibia World Script Maker (Libs Scripts)

function getPlayerBalance(cid)
    getcurrentbalance = mysqlQuery("SELECT `bank_balance` FROM `players` WHERE `name` = \"" .. getPlayerName(cid) .. "\"", "bank_balance")
	return getNumberValue(getcurrentbalance.bank_balance)
end

function WithdrawMoney(cid, amount)
	if getPlayerBalance(cid) >= amount then
		mysqlQuery('UPDATE `players` SET `bank_balance`="'.. getPlayerBalance(cid)-amount ..'" WHERE `name` = "' .. getPlayerName(cid) .. '"', 'UPDATE')
		doPlayerAddMoney(cid, amount)
	return TRUE
	end
end

function DepositMoney(cid, amount)
	if doPlayerRemoveMoney(cid, amount) == TRUE then
		mysqlQuery('UPDATE `players` SET `bank_balance`="'.. getPlayerBalance(cid)+amount ..'" WHERE `name` = "' .. getPlayerName(cid) .. '"', 'UPDATE')
	return TRUE
	end
end

function getPlayerMoney(cid)
	return ((getPlayerItemCount(cid, ITEM_CRYSTAL_COIN) * 10000) + (getPlayerItemCount(cid, ITEM_PLATINUM_COIN) * 100) + getPlayerItemCount(cid, ITEM_GOLD_COIN))
end

function doPlayerWithdrawAllMoney(cid)
	return doPlayerWithdrawMoney(cid, getPlayerBalance(cid))
end

function doPlayerDepositAllMoney(cid)
	return doPlayerDepositMoney(cid, getPlayerMoney(cid))
end

function doPlayerTransferAllMoneyTo(cid, target)
	return doPlayerTransferMoneyTo(cid, target, getPlayerBalance(cid))
end