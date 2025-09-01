--- @meta

--- @class vector: number[]
--- @field items number[]
--- @field len integer
--- @operator add(vector): vector
--- @operator sub(vector): vector
--- @operator mul(number): vector
--- @operator div(number): vector
--- @operator unm(): vector
local vector_methods = {}

local vector = {}

--- @param ... number
--- @return vector
vector.new = function(...) end

--- Creates vector from its hexadecimal representation; each coordinate is between 0 and 1
--- @param hex string
--- @return vector
vector.hex = function(hex) end

vector.zero = vector.new(0, 0)
vector.one = vector.new(1, 1)
vector.up = vector.new(0, -1)
vector.down = vector.new(0, 1)
vector.left = vector.new(-1, 0)
vector.right = vector.new(1, 0)

vector.white = vector.new(1, 1, 1)
vector.black = vector.new(0, 0, 0)

--- @alias direction_name "up" | "left" | "down" | "right"

vector.direction_names = {"up", "left", "down", "right"}
vector.directions = {vector.up, vector.left, vector.down, vector.right}
vector.extended_directions = {
  vector.up, vector.left, vector.down, vector.right,
  vector.new(1, 1), vector.new(1, -1), vector.new(-1, -1), vector.new(-1, 1)
}

--- @param v vector
--- @return string?
vector.name_from_direction = function(v) end

-- --- @param f fun(n: number): number
-- --- @param ... vector
-- --- @return vector
-- vector.use = function(f, ...) end

--- @generic T
--- @param self T
--- @return T
vector_methods.copy = function(self) end

--- @generic T
--- @param self T
--- @param other vector
--- @return T
vector_methods.add_mut = function(self, other) end

--- @generic T
--- @param self T
--- @param other vector
--- @return T
vector_methods.sub_mut = function(self, other) end

--- @generic T
--- @param self T
--- @param other number
--- @return T
vector_methods.mul_mut = function(self, other) end

--- @generic T
--- @param self T
--- @param other number
--- @return T
vector_methods.div_mut = function(self, other) end

--- @generic T
--- @param self T
--- @param other number
--- @return T
vector_methods.mod_mut = function(self, other) end

--- @generic T
--- @param self T
--- @return T
vector_methods.unm_mut = function(self) end

--- @param self vector
--- @param f fun(n: number): number
--- @return vector
vector_methods.map_mut = function(self, f) end

--- @param self vector
--- @param f fun(n: number): number
--- @return vector
vector_methods.map = function(self, f) end

--- @param self vector
--- @return number
vector_methods.abs = function(self) end

--- @param self vector
--- @return integer
vector_methods.abs2 = function(self) end

--- @param self vector
--- @return vector
vector_methods.normalized = function(self) end

--- @param self vector
--- @return vector
vector_methods.normalized2 = function(self) end

--- @param self vector
--- @return vector
vector_methods.normalized_mut = function(self) end

--- @param self vector
--- @return vector
vector_methods.normalized2_mut = function(self) end

--- @param self vector
--- @param pattern string
--- @return vector
vector_methods.normalized2_mut = function(self, pattern) end

return vector
