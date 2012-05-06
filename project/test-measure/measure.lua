-- устанавливаем частоты измерений процессов
second = 1000000
-- температура
measurementFrequencyTemperature = 10 * second
-- давление
measurementFrequencyPreasure = 60 * second
-- освещенность
measurementFrequencyIllumination = 30 * second

function StartUpTime()
   return 100
end

function initializeNode()
   -- устанавливаем таймеры
   innerInterraptInsert(10 * second, "timer")
   dataSet("Timer interrupt counter", 0)
end

function eventActionInner(type)
   local timerInterruptCounter = dataGet("Timer interrupt counter")
   timerInterruptCounter = timerInterruptCounter + 1

   measure("temperature")

   if (timerInterruptCounter % 3 == 0) then
      measure("pressure")
   end

   if (timerInterruptCounter % 6 == 0) then
      measure("illuminance")
   end

   dataSet("Timer interrupt counter", timerInterruptCounter)
   innerInterraptInsert(10 * second, "timer")
end
