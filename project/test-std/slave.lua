require("networkConsts")
require("bit")

-- возможные состояния радио узла
rstNone = 0
rstAssocReqSent = 1
rstWorking = 2

function initHardware()
   -- установка на приемо-передатчика длинного адреса узла
   wpanInit()
   -- установка PAN_ID
   wpanSetPANID(PAN_ID)

   -- начальное состояние радио на узле
   radioState = rstNone

   -- устанавливаем таймер на 500 мкс
   timerInterruptInsert(500, "init")
end

function timerInterruptHandler(type)
   -- прерывание по таймеру

   -- если была попытка отправить запрос на подключение к сети, но ответа не пришло
   if (radioState == rstAssocReqSent) then
      -- сбрасываем состояние радио
      radioState = rstNone
   end

   -- если узел не был включен в сеть
   if (radioState ~= rstWorking) then

      -- отправляем координатору запрос на подключение к сети
      wpanAssocReq(COORD_ADDR)
      
      -- меняем состояние радио на rstAssocReqSent
      radioState = rstAssocReqSent
      
      -- ожидаем ответ в течение 10 мс
      timerInterruptInsert(10000, "")
   end
end

function radioInterruptHandler(message)
   -- прерывание по приему сообщения
   -- получаем тип сообщения
   messageType = bit.band(message[1], 3)

   -- если тип сообщения wpan_mtMAC
   if (messageType == wpan_mtMAC) then
      -- если сообщение является ответом на запросом присоединения к сети и узел ожидаел этот ответ
      if ((message[24] == 2) and (radioState == rstAssocReqSent)) then
         -- обрабатываем сообщение
         processing()

         -- смотрим какой адрес нам выдал координатор
         local myAddr = message[26]
         -- не работает
         -- myAddr = bit.blshift(myAddr, 8)
         myAddr = myAddr + message[25]
         -- если адрес не является недопустимым
         if (myAddr ~= 0xFFFF) then
            -- устанавливаем его
            wpanSetShortAddr(myAddr)
            
            -- переходим в рабочее состояние
            radioState = rstWorking

            -- и отправляем сообщение с данными без требования ответа
            wpanSendData({1,2,3,4}, 0, false)
         end
      end
   end
end
