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
  assert(rawequal(v, u))
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

do
  print("Equality")
  local v = vector.new(1, 2, 3)
  local u = vector.new(1, 2, 3)
  local n = vector.new(1, 2, 3, 0)
  local m = vector.new(2, 2, 2)

  assert(v == u)
  assert(v ~= n)
  assert(v ~= m)
end

do
  print("Copy")
  local v = vector.new(1, 2, 3)
  local u = v:copy()

  assert(u == v)
  v.x = 3
  assert(v.x == 3)
  assert(u.x == 1)
end

do
  print("Immutable arithmetic")
  local v = vector.new(1, 2, 3)
  local u = vector.new(3, 3, 3)
  assert(v + u == vector.new(4, 5, 6))
  assert(v == vector.new(1, 2, 3))
  assert(u == vector.new(3, 3, 3))

  assert(v - u == vector.new(-2, -1, 0))

  assert(v * 2 == vector.new(2, 4, 6))
  assert(u / 3 == vector.new(1, 1, 1))
end

do
  print("Unpack")
  local v = vector.new(1, 3, 3, 7)
  local x, y, z, w = v:unpack()
  assert(x == 1)
  assert(y == 3)
  assert(z == 3)
  assert(w == 7)
end

do
  print("Swizzle")
  local v = vector.new(1, 3, 3, 7)
  assert(v:swizzle("arb") == vector.new(7, 1, 3))
end
