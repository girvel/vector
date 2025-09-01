local ffi = require("ffi")

--- @class vector
--- @field items number[]
--- @field len integer
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

    vector *vector_unm_mut(vector *self);
    vector *vector_add_mut(vector *self, const vector *other);
    vector *vector_sub_mut(vector *self, const vector *other);
    vector *vector_mul_mut(vector *self, double k);
    vector *vector_div_mut(vector *self, double k);
    vector *vector_mod_mut(vector *self, double k);

    bool vector_eq(const vector *self, const vector *other);
    bool vector_lt(const vector *self, const vector *other);
    bool vector_le(const vector *self, const vector *other);

    double vector_abs(const vector *self);
    double vector_abs2(const vector *self);
    vector *vector_normalized_mut(vector *self);
    vector *vector_normalized2_mut(vector *self);

    vector *vector_swizzle(const vector *self, const char *swizzle_str, vector *result);
    const char* vector_name_from_direction(const vector *self);
    bool vector_from_hex(const char *hex_str, vector *result);
]]

local vector_methods = {}
vector.mt = {}
vector.mt.__index = vector_methods
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

vector_methods.copy = function(self)
  local v = vector_cdata_type()
  v.len = self.len
  for i = 0, v.len - 1 do
    v.items[i] = self.items[i]
  end
  return v
end

vector_methods.add_mut = C.vector_add_mut
vector_methods.sub_mut = C.vector_sub_mut
vector_methods.mul_mut = C.vector_mul_mut
vector_methods.div_mut = C.vector_div_mut

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
