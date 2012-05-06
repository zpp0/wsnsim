-- подгружаем дополнительные модули
require("bit")
require("random")
require("networkConsts")

function timeOfPowerOn()
   local mu = 1000000
   local sigma = 500000
   return normalDistribution(mu, sigma)
--     return 0
end

-- константы алгоритма CSMA-CA
macMinBE = 3
aMaxBE = 5
macMaxCSMABacoffs = 4
aUnitBackoffPeriod = 20

-- инициализация переменных алгоритма CSMA-CA
function macCSMADataInit()
   -- количество неуспешных попыток передачи текущего сообщения
   NB = 0
   -- показатель степени числа 2, определяющий продолжительность интервала (в единичных паузах), который устройство должно ждать перед доступом к каналу
   BE = macMinBE
end

-- функция возвращает случайный адрес из диапазона [0, maxAddr] исключая собственный адрес узла
function generateRandomAddr(maxAddr, myAddr)
   -- генерируем адрес назначения случайным образом с равномерным распределением от 0 до maxAddr
   -- задаем адрс назначения собственным адресом узла
   local shortDestAddr = myAddr
   -- генерируем новый адрес до тех пор, пока не перестанет совпадать с собственным
   while shortDestAddr == myAddr do
      shortDestAddr = math.random(maxAddr)
   end

   -- возвращаем сгенерированное значение
   return shortDestAddr
end   

-- попытка отправить сообщение с использованием алгоритма CSMA-CA
-- сообщение отправляется на адрес shortDestAddr
function attemptSendDataWithCSMA()

   -- генерируем случайный адрес назначения
   shortDestAddr = generateRandomAddr(MAX_SHORT_ADDR, myShortAddr)

   -- инициализируем алгоритм CSMA-CA
   macCSMADataInit()

   -- записываем данные в буфер
   macCSMADataBuffer = { 10, 11, 12, 13, 14 }
   -- записываем адрес назначения в буфер
   macCSMADstShortAddr = shortDestAddr

   -- устанавливаем прерывание по таймеру
   macCSMADelay()
end

-- функция устанавливает таймер задержки алгоритма CSMA-CA
function macCSMADelay()
   -- вычисляем задержку в периодах
   local backoffPeriods = aUnitBackoffPeriod * math.random(2^BE - 1)
   -- переводим периоды в мкс
   local backoffTime = backoffPeriods * 16

   -- устанавливаем таймер
   timerInterruptInsert(backoffTime, CSMAtimerHandler)
end

-- функция инициализции программы узла
function init()

   -- получаем собственный идентификационный номер
   ID = dataGet("ID")
   -- собственный короткий адрес будет равен идентификационному номеру
   myShortAddr = ID
   -- устанавливаем короткий адрес на приемнике-передатчике
   wpanSetShortAddr(myShortAddr)

   -- переменная содержит номер последнего отправленного сообщения или -1, если на сообщение был получен ответ
   -- номер сообщения входит в диапазон [0, 255]
   ack = -1

   -- устанавливаем состояние генератора псевдослучайных чисел
   math.randomseed(ID)

   -- пытаемся отправить сообщение на случайный адрес с использованием алгоритма CSMA-CA
   attemptSendDataWithCSMA()
end

-- обработчик прерывания по таймеру
function timerInterruptHandler()
   -- по алгоритму CSMA-CA после задержки проверяем состояние канала
   if (CCA() == 1) then
      -- канал свободен
      -- отправляем сообщение с запросом ответа, запоминаем его номер
      ack = wpanSendData(macCSMADataBuffer, macCSMADstShortAddr, true)

      -- пытаемся отправить новое сообщение
      attemptSendDataWithCSMA()
   else
      -- канал занят
      -- увеличиваем количество неуспешных попыток отправить сообщение
      NB = NB + 1

      -- увеличиваем экспоненту
      BE = math.min(BE + 1, aMaxBE)

      -- если количество неуспешных попыток не превышает максимальное
      if (NB < macMaxCSMABacoffs) then
         -- устанавливаем задержку
         macCSMADelay()
      else
         -- если превышает - пытаемся отправить новое сообщение

         -- пытаемся отправить сообщение на случайный адрес с использованием алгоритма CSMA-CA
         attemptSendDataWithCSMA()
      end            
   end
end

-- обработчик прерывания приема сообщения по радио
function radioInterruptHadler(message)
   -- получаем адрес назначения этого сообщения
   local dstAddr = wpanGetShortDstAddressFromMessage(message)
   -- его номер
   local seqNum = message[3]
   -- и тип сообщения
   local messageType = bit.band(message[1], 7)

   -- если это сообщение с данными
   if (messageType == wpan_mtData) then
      -- и адрес назначения собпадает с собственным
      if (dstAddr == math.abs(myShortAddr)) then
         -- оповещаем среду о получении полезного сообщения
         processing()
         -- и отправляем ответ, содержащий номер полученного сообщения
         wpanSendAck(seqNum)
      end
      -- если тип сообщения соответствует ответу
   else if (messageType == wpan_mtAck) then
         -- если номер последовательности совпадает с ожидаемым - пришел ответ на прошлое отправленное сообщение узла
         if (ack == seqNum) then
            -- оповещаем среду о получении полезного сообщения
            processing()
            -- сбрасываем переменную номера последнего отправленного сообщения
            ack = -1
         end
      end
   end
end
