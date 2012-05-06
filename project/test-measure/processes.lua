function temperature (x, y, z, time)
   return math.sqrt(x*x + y*y + z*z)
end

function illuminance (x, y, z, time)
   if (time > 360000000) then
      return 45
   else
      return 5
   end
end

function pressure (x, y, z, time)
   return math.log(time)
end