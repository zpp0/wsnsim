-- подгружаем модуль с описанием параметров сети
require ("network")

function initializeNode()
end

function eventActionRadio(destanationNodeID, message)
   if (message[1] == RFCOMMAND_MEASURE_REQUIRE) then

      if (message[2] == TEMPERATYRE_PROC_ID) then
         processing()
         local procValue = measure("temperature")
         physicMesSend(MASTER_ID, { RFCOMMAND_MEASURE_RESULT, 
                                    TEMPERATYRE_PROC_ID,
                                    procValue })

      elseif (message[2] == ILLUMINANCE_PROC_ID) then
         processing()
         local procValue = measure("illuminance")
         physicMesSend(MASTER_ID, { RFCOMMAND_MEASURE_RESULT,
                                    ILLUMINANCE_PROC_ID,
                                    procValue })

      elseif (message[2] == PRESSURE_PROC_ID) then
         processing()
         local procValue = measure("pressure")
         physicMesSend(MASTER_ID, { RFCOMMAND_MEASURE_RESULT,
                                    PRESSURE_PROC_ID,
                                    procValue })

      end
   end
end
