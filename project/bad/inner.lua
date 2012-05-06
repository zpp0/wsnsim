function initializeNode ()
   -- asd = measure("lift")
   -- print ("asd")   
   -- log("succsess init "..dataGet("MAC"))
   -- log("succsess init ")
   innerInterraptInsert(100, "init")
end

function a123(x, y, z, time)
   if time == 100 then
      return 5
   end
   if time > 100 then
      return -100
   end
end

function movement(x, y, z, time)
   xn = 0
   yn = 0
   zn = 0
   if time < 707000 then
      yn = y+time/1000000 * 2 -- V = 2м/с
   end
   -- print ("LUA V " .. xn .. " " .. yn .. " " .. zn)
   return xn, yn, zn
end

function eventActionInner(type)

   -- print("LUA ".. type .. " " .. dataGet("MAC"))

   -- log('interrupt '..type)
   time1 = getLocalTime()
   -- print("time" .. time1)

   -- if type=="init" then
   --    if dataGet("MAC") > "1" then
   --       -- print ("measure")
   --       measure("a23")
   --    end
   -- end

   -- if type=="test" then
   --    innerInterraptInsert(500, "test")
   -- end

   -- if dataGet("MAC") == "0" then
   physicMesSend(1,{1, 2, 3})
   -- innerInterraptInsert(500, "test")
   -- end

end

function G (nu, fi, time)
   return 0.9
end

function eventActionRadio(reciver,mesBody)
   -- print("LUA radio on " .. dataGet("MAC") .. " node")
--   MAC = dataGet("MAC")
   processing()
--   measure("a23")
   innerInterraptInsert(500, "test")
end

function localtime(time, t)
   return time+t
end

function globaltime(time, t)
   return time-t
end