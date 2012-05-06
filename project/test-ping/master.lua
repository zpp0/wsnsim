-- подгружаем модуль с описанием параметров сети
package.path = package.path .. ";" .. "/home/zpp/simulator/project/test-ping/?.lua"
require ("network")

function initializeNode()
   -- узел проснулся
   innerInterraptInsert(1000000, "timer")
   physicMesSend(SLAVE_ID, { RFCOMMAND_PING })
end

function eventActionInner(type)
   physicMesSend(SLAVE_ID, { RFCOMMAND_PING })
end

function eventActionRadio(destanationNodeID, message)
   if (message[1] == RFCOMMAND_PONG) then
      processing()
   end
end