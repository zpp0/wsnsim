function normalDistribution(mu, sigma)
   local x = math.random()
   local y = math.random()

   local z = math.cos(2 * math.pi * x) * math.sqrt(-2 * math.log(y))
   return (mu + sigma * z)
end
