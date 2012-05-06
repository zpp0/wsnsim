require ("random")

function initializeNode()
    -- узел проснулся
    MACadr = dataGet("ID")
    timerInterruptInsert(500, "init")
    math.randomseed(dataGet("ID"))
end

function eventActionInner(type)
    if (type == "init") then
        rfMesSend({ MACadr })
        local waitTime = random.normalDistribution(1000, 500)
        timerInterruptInsert(waitTime, "timer")
    elseif (type == "timer") then
        rfMesSend({ MACadr, 1 })
    end
end