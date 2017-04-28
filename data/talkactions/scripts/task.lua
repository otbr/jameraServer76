function onSay(cid, words, param, target)
        param =  param:lower()
        local task = getTaskMission(cid)
        local daily = getDailyTaskMission(cid)
        if (isInArray({"counter","contador"},param) == TRUE) then
                setPlayerStorageValue(cid, task_sys_storages[8], getPlayerStorageValue(cid, task_sys_storages[8]) <= 0 and 1 or 0)
                doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE,"[Task System] O contador foi "..(getPlayerStorageValue(cid, task_sys_storages[8]) <= 0 and "ativado" or "desativado")..".") return true
        elseif (isInArray({"daily","diaria"},param) == TRUE) then
                if not daily_task[daily] or getPlayerStorageValue(cid, task_sys_storages[7]) <= 0 then
                        doPlayerSendCancel(cid, "Desculpe, Mas você não está em nenhuma Daily Task.") return true
                elseif getPlayerStorageValue(cid, task_sys_storages[6]) - os.time() <= 0 and getPlayerStorageValue(cid, task_sys_storages[5]) < daily_task[daily].count then
                        doPlayerSendCancel(cid,"Desculpe, Mas Você não terminou a Daily Task a tempo! Por favor volte ao npc e comece uma nova Daily Task!") return true
                end
                return doShowTextDialog(cid, 1950, "[->] CURRENT DAILY TASK INFO [<-]\n\nNome: "..daily_task[daily].name.."\nProgresso: ["..(getPlayerStorageValue(cid, task_sys_storages[5]) < 0 and 0 or getPlayerStorageValue(cid, task_sys_storages[5])).."/"..daily_task[daily].count.."]\nPrazo para entrega: "..os.date("%d %B %Y %X ", getPlayerStorageValue(cid,task_sys_storages[6])).."\nMonstros para caçar: "..getMonsterFromList(daily_task[daily].monsters_list).."\n\n[->] CURRENT TASK REWARDS [<-]\n\nMoney: "..(daily_task[daily].money > 0 and daily_task[daily].money or 0).."\nExperiencia: "..(daily_task[daily].exp > 0 and daily_task[daily].exp or 0).."\nTask Points: "..daily_task[daily].points.."\nItems: "..(#daily_task[daily].reward > 0 and getItemsFromList(daily_task[daily].reward) or "Nenhum item de recompensa")..".")
        end
        if not task_sys[task] or getPlayerStorageValue(cid, task_sys[task].start) <= 0 then
                doPlayerSendCancel(cid, "você não está em nenhuma task.") return true
        end
        return doShowTextDialog(cid, 1950, "-> CURRENT TASK ["..getTaskMission(cid).."/"..#task_sys.."] <-\n\nTask Name: "..task_sys[task].name.."\nTask Level: "..task_sys[task].level.."\nTask Progress: ["..(getPlayerStorageValue(cid, task_sys_storages[3]) < 0 and 0 or getPlayerStorageValue(cid, task_sys_storages[3])).."/"..task_sys[task].count.."]\nMonster To Hunt: "..getMonsterFromList(task_sys[task].monsters_list)..".\nItens Para Entrega: "..(#task_sys[task].items > 0 and getItemsFromList(task_sys[task].items) or "Nenhum")..".\n\n[->] CURRENT TASK REWARDS [<-]\n\nReward Money: "..(task_sys[task].money > 0 and task_sys[task].money or 0).."\nReward Experiencia: "..(task_sys[task].exp > 0 and task_sys[task].exp or 0).."\nReward Points: "..task_sys[task].points.."\nRedward Items: "..(#task_sys[task].reward > 0 and getItemsFromList(task_sys[task].reward) or "Nenhum item de recompensa")..".")
end

