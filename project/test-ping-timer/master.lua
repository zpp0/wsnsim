-- подгружаем модуль с описанием параметров сети
require ("network")
require ("random")

function timeForWait()
   local mu = 10000000
   local sigma = 1500000
   return normalDistribution(mu, sigma)
end

function initializeNode()
   -- узел проснулся
   innerInterraptInsert(timeForWait(), "timer")
end

function eventActionInner(type)
   physicMesSend(SLAVE_ID, { RFCOMMAND_PING })

   innerInterraptInsert(timeForWait(), "timer")
end

function eventActionRadio(destanationNodeID, message)
   if (message[1] == RFCOMMAND_PONG) then
      processing()
   end
end