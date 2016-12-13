function setExhaust(cid, storage)
    setPlayerStorageValue(cid, storage, os.time())
end

function isExhausted(cid, storage, exhaust)
    local exhaustTime = getPlayerStorageValue(cid, storage)
    if exhaustTime == -1 then
        return FALSE
    end
    local isExhausted = os.time() - exhaustTime < exhaust
    return isExhausted and TRUE or FALSE
end

function exhaust(cid, storevalue, exhausttime)
-- Exhaustion function by Alreth, v1.1 2006-06-24 01:31
-- Returns 1 if not exhausted and 0 if exhausted
    newExhaust = os.time()
    oldExhaust = getPlayerStorageValue(cid, storevalue)
    if (oldExhaust == nil or oldExhaust < 0) then
        oldExhaust = 0
    end
    if (exhausttime == nil or exhausttime < 0) then
        exhausttime = 1
    end
    diffTime = os.difftime(newExhaust, oldExhaust)
    if (diffTime >= exhausttime or diffTime < 0) then
        setPlayerStorageValue(cid, storevalue, newExhaust) 
        return 1
    else
        return 0
    end
end

