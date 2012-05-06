function initializeNode()
    innerInterraptInsert(500, "init")
end

function eventActionInner(type)
    
    physicMesSend(0, { dataGet("MAC") })
end