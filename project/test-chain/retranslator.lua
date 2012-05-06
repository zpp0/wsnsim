-- подгружаем модуль с реализацией псевдослучайных величин
require ("network")

function initializeNode()
end

function eventActionRadio(destanationNodeID, message)
   -- собственный MAC адрес узла
   local MACaddress = math.abs(dataGet("MAC"))

   -- пришел запрос на измерение
   if (message[1] == RFCOMMAND_MEASURE) then

      -- если MAC адрес больше чем адрес отправителя
      if (message[2] < MACaddress) then
         -- узел обрабатывает сообщение
         processing()
         -- путем ретранслирования сообщения с заменой адреса отправителя на собственный
         physicMesSend(reciver, {RFCOMMAND_MEASURE, MACaddress, reciver})
      end
   end

   -- пришел ответ на запрос на проведение изменения
   if (message[1] == RFCOMMAND_MEASURE_ACK) then

      -- если MAC адрес меньше чем адрес отправителя
      if (message[2] > MACaddress) then
         -- узел обрабатывает сообщение
         processing()
         -- путем ретранслирования сообщения с заменой адреса отправителя на собственный
         physicMesSend(0, {RFCOMMAND_MEASURE_ACK, MACaddress, 0, mesBody[4]})
      end
   end
end