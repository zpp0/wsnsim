require("networkConsts")
require("bit")

function initHardware()
   -- установка на приемо-передатчика длинного адреса узла
   wpanInit()
   -- установка PAN_ID
   wpanSetPANID(PAN_ID)

   -- устанавливаем таймер на 500 мкс
   timerInterruptInsert(500, "init")
end

function timerInterruptHandler(type)
   -- прерывание по таймеру

   -- отправляем координатору запрос на подключение к сети
   wpanAssocReq(COORD_ADDR)
end

function radioInterruptHandler(message)
   -- прерывание по приему сообщения
   -- получаем тип сообщения
   messageType = bit.band(message[1], 3)

   -- если тип сообщения wpan_mtMAC
   if (messageType == wpan_mtMAC) then
      -- и сообщение является ответом на запросом присоединения к сети
      if (message[24] == 2) then
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
            -- и отправляем сообщение с данными без требования ответа
            wpanSendData({1,2,3,4}, 0, false)
         end
      end
   end
end
