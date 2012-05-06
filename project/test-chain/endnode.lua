-- подгружаем модуль с реализацией псевдослучайных величин
require ("network")

function initializeNode()
   -- узел проснулся
end

function eventActionRadio(destanationNodeID, message)
   -- пришел запрос на измерение
   if (message[1] == RFCOMMAND_MEASURE) then
      -- узел может обработать это сообщение
      processing()
      -- узел проводит измерение
      local t = measure("temperature")
      -- и отправляет его на узел 0
      physicMesSend(0, {RFCOMMAND_MEASURE_ACK, MACaddress, 0, t})
   end

   -- пришел ответ на запрос на проведение изменения
   if (message[1] == RFCOMMAND_MEASURE_ACK) then
      -- узел может обработать это сообщение
      processing()
      -- узел засыпает на 10 секунд
      innerInterraptInsert(timeForWait(), "timer")
   end

end
