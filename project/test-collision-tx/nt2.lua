function initializeNode()
    innerInterraptInsert(480, "init")
end

function eventActionInner(type)
    physicMesSend(1, { dataGet("MAC") })
end