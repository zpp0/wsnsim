-- подгружаем модуль с описанием параметров сети
-- package.path = package.path .. ';' .. '/home/zpp/simulator/project/test-measure-radio/?.lua'
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

function movement(x, y, z, time)

   -- центр круга
   local Xc = 5.0
   local Yc = 5.0

   -- радиус
   local R = 3.0
   -- период
   local T = 30000000
   
   -- время на текущем круге
   local circle_time = math.mod(time, T)

   -- координата по х
   local xn = math.cos((2.0 * math.pi * circle_time) / T) * R + Xc
   -- координата по у
   local yn = math.sin((2.0 * math.pi * circle_time) / T) * R + Yc
   local zn = 0.0

   return xn, yn, zn
end
