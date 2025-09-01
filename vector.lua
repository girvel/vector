local ffi = require("ffi")

--- @class vector: number[]
--- @field x number alias for `[1]`
--- @field y number alias for `[2]`
--- @field z number alias for `[3]`
--- @field w number alias for `[4]`
--- @field r number alias for `[1]`
--- @field g number alias for `[2]`
--- @field b number alias for `[3]`
--- @field a number alias for `[4]`
--- @operator add(vector): vector
--- @operator sub(vector): vector
--- @operator mul(number): vector
--- @operator div(number): vector
--- @operator unm(): vector
local vector = {}


local C = ffi.load("./libvector.so")
ffi.cdef[[
    typedef struct {
        int len;
        double items[4];
    } vector;

    bool vector_eq(const vector *self, const vector *other);
]]

local vector_methods = {}
vector.mt = {}
vector.mt.__eq = C.vector_eq

local vector_cdata_type = ffi.metatype("vector", vector.mt)


vector.new = function(...)
    local n = select('#', ...)
    if n > 4 then
        error("Too many arguments, max is " .. 4)
    end

    local v = vector_cdata_type()
    v.len = n

    for i = 1, n do
        v.items[i - 1] = select(i, ...)
    end

    return v
end

vector.mt.__tostring = function(self)
  local result = "{"
  for i = 0, self.len - 1 do
    if i > 0 then
      result = result .. "; "
    end
    result = result .. self.items[i]
  end
  return result .. "}"
end

return vector
