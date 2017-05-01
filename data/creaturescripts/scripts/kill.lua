

function onKill(cid, target)
	if isPlayer(cid) == TRUE and isMonster(target) == TRUE then
                local t,daily = task_sys[getTaskMission(cid)], daily_task[getDailyTaskMission(cid)]
                if t and getPlayerStorageValue(cid, t.start) > 0 and isInArrayString(t.monsters_list, getCreatureName(target):lower())  and getPlayerStorageValue(cid, task_sys_storages[3]) < t.count then
                        setPlayerStorageValue(cid, task_sys_storages[3], getPlayerStorageValue(cid, task_sys_storages[3]) < 0 and 1 or (getPlayerStorageValue(cid, task_sys_storages[3])+1))
                        if getPlayerStorageValue(cid, task_sys_storages[3]) < t.count then
                                doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_ORANGE,"[Task System] defeated Total [" .. getPlayerStorageValue(cid, task_sys_storages[3]) .. "/" .. t.count .. "] da Task do " .. t.name .. ".")
                        end
                        if getPlayerStorageValue(cid, task_sys_storages[3]) >= t.count then
                                doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_ORANGE,"[Task System] Parabens! Voce terminou a Task do "..t.name..", volte ao npc parece receber sua recompensa.")
                        end
                end
                if daily and getPlayerStorageValue(cid, task_sys_storages[7]) > 0 and getPlayerStorageValue(cid, task_sys_storages[6]) - os.time() >= 0 and isInArrayString(daily.monsters_list, getCreatureName(target):lower()) and getPlayerStorageValue(cid, task_sys_storages[5]) < daily.count then
                        setPlayerStorageValue(cid, task_sys_storages[5], getPlayerStorageValue(cid, task_sys_storages[5]) < 0 and 1 or (getPlayerStorageValue(cid, task_sys_storages[5])+1))
                        if getPlayerStorageValue(cid, task_sys_storages[5]) < daily.count then
                                doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE,"[Daily Task System] defeated Total [" .. getPlayerStorageValue(cid, task_sys_storages[5]) .. "/" .. daily.count .. "] da Task do " .. daily.name .. ".")
                        end
                        if getPlayerStorageValue(cid, task_sys_storages[5]) >= daily.count then
                                doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_ORANGE,"[Daily Task System] Parabens! Voce terminou a Task do "..daily.name..", volte ao npc parece receber sua recompensa.")
                        end
                end
        end

	return 1

end
