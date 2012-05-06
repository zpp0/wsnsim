-- подгружаем модуль с реализацией псевдослучайных величин
require ("random")
require ("network")

function timeForWait()
   local mu = 10000000
   local sigma = 1500000
   return normalDistribution(mu, sigma)
end

function initializeNode()
   -- узел проснулся

      -- и засыпает на 10 секунд
      innerInterraptInsert(timeForWait(), "timer")
end

function eventActionRadio(destanationNodeID, message)
   -- пришел ответ на запрос на проведение изменения
   if (message[1] == RFCOMMAND_MEASURE_ACK) then

      -- если адрес назначения совпадает с MAC адресом
      if (reciver == MASTER_ID) then
         -- узел может обработать это сообщение
         processing()
         -- узел засыпает на 10 секунд
         innerInterraptInsert(timeForWait(), "timer")
      end
   end
end

function eventActionInner(type)

   physicMesSend(destanationNodeID, { RFCOMMAND_MEASURE, 
                                      MASTER_ID, 
                                      ENDNODE_ID })
end
