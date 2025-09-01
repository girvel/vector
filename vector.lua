local ffi = require("ffi")
local C = ffi.load("./libvector.so")

ffi.cdef[[
    typedef struct {
        int len;
        double items[4];
    } vector;

    bool vector_eq(const vector *self, const vector *other);
]]

local vector_mt = {}
vector_mt.__eq = C.vector_eq

local vector_cdata_type = ffi.metatype("vector", vector_mt)

local vector = {}

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

return vector
