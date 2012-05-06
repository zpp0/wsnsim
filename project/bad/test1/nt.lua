-- подгружаем модуль с реализацией псевдослучайных величин
package.path = package.path .. ";" .. "/home/zpp/simulator/project/test1/?.lua"
require ("random")

RFCOMMAND_MEASURE = 1
RFCOMMAND_MEASURE_ACK = 2

function timeForWait()
   local mu = 10000000
   local sigma = 1500000
   return random.normalDistribution(mu, sigma)
end

function initializeNode()
   -- узел проснулся

   local MACaddress = dataGet("MAC")

   -- если id узла равно 0,
   if (MACaddress == "0") then
      -- то он получает id измеряющего узла
      dataSet("Destanation Node ID", 9)
      -- и засыпает на 10 секунд
      innerInterraptInsert(timeForWait(), "timer")
      
   elseif (MACaddress == "9") then
      dataSet("Destanation Node ID", 0)
   end

end

function eventActionRadio(destanationNodeID, message)
   -- собственный MAC адрес узла
   local MACaddress = math.abs(dataGet("MAC"))

   -- пришел запрос на измерение
   if (message[1] == RFCOMMAND_MEASURE) then

      -- если адрес назначения совпадает с MAC адресом
      if (destanationNodeID == MACaddress) then
         -- узел может обработать это сообщение
         processing()
         -- узел проводит измерение
         t = measure("t")
         -- и отправляет его на узел 0
         physicMesSend(0, {2, MACaddress, 0, t})

      -- если адрес не совпал, но MAC больше чем адрес отправителя
      elseif (mesBody[2] < MACaddress) then
         -- узел обрабатывает сообщение
         processing()
         -- путем ретранслирования сообщения с заменой адреса отправителя на собственный
         physicMesSend(reciver,{1, MACaddress, reciver})
      end
   end

   -- пришел ответ на запрос на проведение изменения
   if (mesBody[1] == 2) then

      -- если адрес назначения совпадает с MAC адресом
      if (reciver == MACaddress) then
         -- узел может обработать это сообщение
         processing()
         -- узел засыпает на 10 секунд
         innerInterraptInsert(timeForWait(), "timer")

      -- если адрес не совпал,
      elseif (reciver ~= MACaddress) then
         -- и MAC меньше чем адрес отправителя
         if (mesBody[2] > MACaddress) then
            -- узел обрабатывает сообщение
            processing()
            -- путем ретранслирования сообщения с заменой адреса отправителя на собственный
            physicMesSend(0,{2, MACaddress, 0, mesBody[4]})
         end
      end
   end

end

function eventActionInner(type)
   -- узел 
   local MACaddress = dataGet("MAC")
   local destanationNodeID = dataGet("Destanation Node ID")

   if (type=="timer") then
      physicMesSend(destanationNodeID, { RFCOMMAND_MEASURE, 
                                         MACaddress, 
                                         destanationNodeID })
   end
end
