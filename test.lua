local vector = require("vector")

do
  print("Initialization and field access")
  local v = vector.new(10, 20)
  assert(v.x == 10)
  assert(v.y == 20)
  v.x = 20
  assert(v.x == 20)

  local u = vector.new(1, 2, 3, 4)
  assert(u.z == 3)
  assert(u.a == 4)
  u.r = 3
  assert(u.x == 3)
end

do
  print("__tostring")
  local v = vector.new(1, 2, 3)
  assert(tostring(v) == "{1; 2; 3}")
end

do
  print("Mutable arithmetic")
  local v = vector.new(1, 2, 3)
  local u = v:add_mut(vector.new(2, 3, 4))
  assert(u == v)
  assert(u.x == 3)
  assert(u.y == 5)
  assert(u.z == 7)

  v:sub_mut(vector.new(2, 4, 6))
  assert(v.x == 1)
  assert(v.y == 1)
  assert(v.z == 1)

  v:mul_mut(3)
  assert(v.x == 3)
  assert(v.y == 3)
  assert(v.z == 3)

  v:div_mut(3)
  assert(v.x == 1)
  assert(v.y == 1)
  assert(v.z == 1)
end
