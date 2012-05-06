-- подгружаем модуль с описанием параметров сети
package.path = package.path .. ";" .. "/home/zpp/simulator/project/test-ping/?.lua"
require ("network")

function initializeNode()
end

function eventActionRadio(destanationNodeID, message)
   if (message[1] == RFCOMMAND_PING) then
      processing()
      physicMesSend(MASTER_ID, { RFCOMMAND_PONG })
   end
end