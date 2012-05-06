require("networkConsts")
require("bit")

function initHardware()
   -- установка на приемо-передатчика длинного адреса узла
   wpanInit()
   -- узел объявляет себя координатором
   wpanSetPanC(true)
   -- установка PAN_ID
   wpanSetPANID(PAN_ID)
end

function radioInterruptHandler(message)
   -- прерывание по приему сообщения

   -- получаем тип сообщения
   messageType = bit.band(message[1], 3)

   -- если тип сообщения wpan_mtMAC
   if (messageType == wpan_mtMAC) then
      -- и сообщение является запросом на присоединение к сети
      if (message[18] == 1) then
         -- то узел в текущем состоянии может обработать это сообщение
         processing()
         -- выделяем запросившему узлу короткий адрес
         slaveShortAddr = 1
         -- и отправляем извещение о присоединении его к сети
         wpanAssociate(123123123, slaveShortAddr, 0)
      end

   -- если тип сообщения wpan_mtData
   elseif (messageType == wpan_mtData) then
      -- не работает
      -- srcAddr = wpanGetSrcAddressFromMessage(message)
      -- print (srcAddr)
      -- и обратный адрес равен адресу дочернего узла
      if (message[7] == slaveShortAddr) then
         -- обрабатываем сообщение
         processing()
         print ("WIN")
         data = wpanGetDataFromMessage(message)
      end
   end
end