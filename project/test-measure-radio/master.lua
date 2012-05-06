-- подгружаем модуль с описанием параметров сети
require ("network")

-- устанавливаем частоты измерений процессов
second = 1000000
-- температура
measurementFrequencyTemperature = 10 * second
-- давление
measurementFrequencyPressure = 60 * second
-- освещенность
measurementFrequencyIlluminance = 30 * second

function initializeNode()
   -- устанавливаем таймер
   innerInterraptInsert(second * 10, "timer")
   dataSet("Timer interrupts counter", 1)
end

function eventActionInner(type)

   innerInterraptInsert(second * 10, "timer")

   counter = dataGet("Timer interrupts counter")

   physicMesSend(SLAVE_ID, { RFCOMMAND_MEASURE_REQUIRE, 
                             TEMPERATYRE_PROC_ID })

   dataSet("Timer interrupts counter", counter + 1)
end

function eventActionRadio(destanationNodeID, message)
   if (message[1] == RFCOMMAND_MEASURE_RESULT) then

      if (message[2] == TEMPERATYRE_PROC_ID) then
         processing()
         if (counter % 3 == 0) then
               physicMesSend(SLAVE_ID, { RFCOMMAND_MEASURE_REQUIRE, 
                                         ILLUMINANCE_PROC_ID })
         end

      elseif (message[2] == ILLUMINANCE_PROC_ID) then
         processing()

         if (counter % 6 == 0) then
               physicMesSend(SLAVE_ID, { RFCOMMAND_MEASURE_REQUIRE,
                                         PRESSURE_PROC_ID })
         end

      elseif (message[2] == PRESSURE_PROC_ID) then
         processing()

      end
   end
end