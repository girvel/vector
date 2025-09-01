local vector = require("vector")

do
  print("Initialization and field access")
  local v = vector.new(10, 20)
  assert(v.x == 10)
  assert(v.y == 20)
  v.x = 20
  assert(v.x == 20)
end
