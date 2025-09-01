#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include <lua5.3/lauxlib.h>
#include <lualib.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 4

typedef struct {
    int len;
    double items[MAX_LEN];
} vector;

static inline int get_index(char key) {
    if (key == 'x') return 0;
    if (key == 'y') return 1;
    if (key == 'z') return 2;
    if (key == 'w') return 3;
    if (key == 'r') return 0;
    if (key == 'g') return 1;
    if (key == 'b') return 2;
    if (key == 'a') return 3;
    return -1;
}

static inline int check_index(lua_State *L, int argument_i) {
    size_t len;
    const char *key = luaL_checklstring(L, argument_i, &len);
    if (len != 1) return -1;
    return get_index(key[0]);
}

static vector* check_vector(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, "vector_metatable");
    return (vector*)ud;
}

inline static vector *vector_allocate(lua_State *L) {
    vector *result = (vector *)lua_newuserdata(L, sizeof(vector));
    luaL_getmetatable(L, "vector_metatable");
    lua_setmetatable(L, -2);
    return result;
}

static int vector_new(lua_State *L) {
    int n = lua_gettop(L);

    if (n > MAX_LEN) {
        return luaL_error(L, "Got %d arguments, expected %d (x, y)", n, MAX_LEN);
    }

    vector *v = vector_allocate(L);

    v->len = n;
    for (int i = 0; i < n; i++) {
        v->items[i] = luaL_checknumber(L, i + 1);
    }

    return 1;
}

static int vector_hex(lua_State *L) {
    size_t str_len;
    const char *str = luaL_checklstring(L, 1, &str_len);

    if (str_len % 2 != 0) {
        return luaL_error(L, "Hex length should be divisible by 2, got %d", str_len);
    }

    char *endptr;
    long int all_value = strtol(str, &endptr, 16);

    if (*endptr != '\0') {
        return luaL_error(L, "Unable to parse the hex '%s'", str);
    }

    if (errno == ERANGE) {
        return luaL_error(L, "Hex value '%s' too big", str);
    }

    vector *result = vector_allocate(L);
    result->len = str_len / 2;
    for (int i = 0; i < result->len; i++) {
        result->items[i] = ((double) ((all_value >> (8 * i)) % 256)) / 255;
    }

    return 1;
}

static int vector_name_from_direction(lua_State *L) {
    vector *self = check_vector(L, 1);
    if (self->len != 2) {
        lua_pushnil(L);
        return 1;
    }
    if (self->items[0] == 0) {
        if (self->items[1] == 1) {
            lua_pushstring(L, "down");
            return 1;
        }
        if (self->items[1] == -1) {
            lua_pushstring(L, "up");
            return 1;
        }
    } else if (self->items[1] == 0) {
        if (self->items[0] == 1) {
            lua_pushstring(L, "right");
            return 1;
        }
        if (self->items[0] == -1) {
            lua_pushstring(L, "left");
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

static vector *vector_copy_raw(lua_State *L, vector *self) {
    vector *result = vector_allocate(L);
    result->len = self->len;
    for (int i = 0; i < self->len; i++) {
        result->items[i] = self->items[i];
    }

    return result;
}

static int vector_copy(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector_copy_raw(L, self);
    return 1;
}

static int vector_unpack(lua_State *L) {
    vector *self = check_vector(L, 1);
    for (int i = 0; i < self->len; i++) {
        lua_pushnumber(L, self->items[i]);
    }
    return self->len;
}

static int vector_swizzle(lua_State *L) {
    vector *self = check_vector(L, 1);
    size_t swizzle_len;
    const char *swizzle_str = luaL_checklstring(L, 2, &swizzle_len);

    if (swizzle_len > MAX_LEN) {
        return luaL_error(L, "Maximal allowed swizzle length is %d, got %d", MAX_LEN, swizzle_len);
    }

    vector *result = vector_allocate(L);
    result->len = swizzle_len;
    for (size_t i = 0; i < swizzle_len; i++) {
        result->items[i] = self->items[get_index(swizzle_str[i])];
    }

    return 1;
}

static int vector_eq(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        lua_pushboolean(L, 0);
        return 1;
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] != other->items[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int vector_lt(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] >= other->items[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int vector_gt(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] <= other->items[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int vector_le(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] > other->items[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int vector_ge(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] < other->items[i]) {
            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

inline static void vector_add_mut_raw(vector *self, vector *other) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] += other->items[i];
    }
}

inline static void vector_sub_mut_raw(vector *self, vector *other) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] -= other->items[i];
    }
}

inline static void vector_mul_mut_raw(vector *self, double k) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] *= k;
    }
}

inline static void vector_div_mut_raw(vector *self, double k) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] /= k;
    }
}

static int vector_add_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    vector_add_mut_raw(self, other);

    lua_pushvalue(L, 1);
    return 1;
}

static int vector_sub_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);

    if (self->len != other->len) {
        return luaL_error(
            L, "Argument vectors have different lengths %d and %d",
            self->len, other->len
        );
    }

    vector_sub_mut_raw(self, other);

    lua_pushvalue(L, 1);
    return 1;
}

static int vector_mul_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    double k = luaL_checknumber(L, 2);

    vector_mul_mut_raw(self, k);

    lua_pushvalue(L, 1);
    return 1;
}

static int vector_div_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    double k = luaL_checknumber(L, 2);

    vector_div_mut_raw(self, k);

    lua_pushvalue(L, 1);
    return 1;
}

static int vector_add(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);
    vector *result = vector_copy_raw(L, self);
    vector_add_mut_raw(result, other);

    lua_pushvalue(L, -1);
    return 1;
}

static int vector_sub(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *other = check_vector(L, 2);
    vector *result = vector_copy_raw(L, self);
    vector_sub_mut_raw(result, other);

    lua_pushvalue(L, -1);
    return 1;
}

static int vector_mul(lua_State *L) {
    vector *self = check_vector(L, 1);
    double k = luaL_checknumber(L, 2);
    vector *result = vector_copy_raw(L, self);
    vector_mul_mut_raw(result, k);

    lua_pushvalue(L, -1);
    return 1;
}

static int vector_div(lua_State *L) {
    vector *self = check_vector(L, 1);
    double k = luaL_checknumber(L, 2);
    vector *result = vector_copy_raw(L, self);
    vector_div_mut_raw(result, k);

    lua_pushvalue(L, -1);
    return 1;
}

inline static int vector_abs_raw(vector *self) {
    double result = 0;

    for (int i = 0; i < self->len; i++) {
        result += self->items[i] * self->items[i];
    }

    return sqrt(result);
}

static int vector_abs(lua_State *L) {
    vector *self = check_vector(L, 1);
    lua_pushnumber(L, vector_abs_raw(self));
    return 1;
}

inline static int vector_abs2_raw(vector *self) {
    double result = 0;

    for (int i = 0; i < self->len; i++) {
        result += fabs(self->items[i]);
    }

    return result;
}

static int vector_abs2(lua_State *L) {
    vector *self = check_vector(L, 1);
    lua_pushnumber(L, vector_abs2_raw(self));
    return 1;
}

static int vector_normalized_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector_div_mut_raw(self, vector_abs_raw(self));
    return 1;
}

static int vector_normalized2_mut(lua_State *L) {
    vector *self = check_vector(L, 1);

    if (self->len != 2) {
        return luaL_error(
            L, "vector:normalized2 works only with two-dimensional vectors, got %d", self->len
        );
    }

    if (fabs(self->items[0]) > fabs(self->items[1])) {
        self->items[0] = copysign(1, self->items[0]);
        self->items[1] = 0;
    } else if (self->items[1] != 0) {
        self->items[0] = 0;
        self->items[1] = copysign(1, self->items[1]);
    } else {
        return luaL_error(L, "Can not normalize zero vector");
    }

    return 1;
}

static int vector_normalized(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *result = vector_copy_raw(L, self);
    vector_div_mut_raw(result, vector_abs_raw(result));
    lua_pushvalue(L, -1);
    return 1;
}

static int vector_normalized2(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *result = vector_copy_raw(L, self);

    if (self->len != 2) {
        return luaL_error(
            L, "vector:normalized2 works only with two-dimensional vectors, got %d", self->len
        );
    }

    if (fabs(self->items[0]) > fabs(self->items[1])) {
        result->items[0] = copysign(1, self->items[0]);
        result->items[1] = 0;
    } else if (self->items[1] != 0) {
        result->items[0] = 0;
        result->items[1] = copysign(1, self->items[1]);
    } else {
        return luaL_error(L, "Can not normalize zero vector");
    }

    lua_pushvalue(L, -1);
    return 1;
}

inline static int vector_map_mut_raw(lua_State *L, vector *self, int argument_i) {
    luaL_checktype(L, argument_i, LUA_TFUNCTION);
    
    for (int i = 0; i < self->len; i++) {
        lua_pushvalue(L, argument_i);
        lua_pushnumber(L, self->items[i]);
        int status = lua_pcall(L, 1, 1, 0);
        if (status != LUA_OK) {
            return lua_error(L);
        }
        double result = luaL_checknumber(L, -1);
        self->items[i] = result;
        lua_pop(L, 1);
    }

    return 1;
}

static int vector_map_mut(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector_map_mut_raw(L, self, 2);
    return 1;
}

static int vector_map(lua_State *L) {
    vector *self = check_vector(L, 1);
    vector *result = vector_copy_raw(L, self);
    vector_map_mut_raw(L, result, 2);
    return 1;
}

static int vector_tostring(lua_State *L) {
    vector *v = check_vector(L, 1);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaL_addstring(&b, "{");
    for (int i = 0; i < v->len; i++) {
        lua_pushnumber(L, v->items[i]);
        luaL_addvalue(&b);
        if (i < v->len - 1) {
            luaL_addstring(&b, "; ");
        }
    }
    luaL_addstring(&b, "}");
    luaL_pushresult(&b);
    return 1;
}

static int vector_len(lua_State *L) {
    vector *v = check_vector(L, 1);
    lua_pushnumber(L, v->len);
    return 1;
}

static int vector_index(lua_State *L) {
    vector *v = check_vector(L, 1);
    int index = check_index(L, 2);
    if (index != -1) {
        lua_pushnumber(L, v->items[index]);
        return 1;
    }

    luaL_getmetatable(L, "vector_metatable");
    lua_pushvalue(L, 2);
    lua_rawget(L, -2);

    return 1;
}

static int vector_newindex(lua_State *L) {
    vector *v = check_vector(L, 1);
    int index = check_index(L, 2);
    double value = luaL_checknumber(L, 3);

    if (index == -1) {
        const char *key = luaL_checkstring(L, 2);
        return luaL_error(L, "Cannot set invalid field '%s' on a vector", key);
    }

    v->items[index] = value;
    return 0;
}

static const struct luaL_Reg vector_methods[] = {
    { "copy", vector_copy },
    { "unpack", vector_unpack },
    { "swizzle", vector_swizzle },
    { "add_mut", vector_add_mut },
    { "sub_mut", vector_sub_mut },
    { "mul_mut", vector_mul_mut },
    { "div_mut", vector_div_mut },
    { "__eq", vector_eq },
    { "__lt", vector_lt },
    { "__gt", vector_gt },
    { "__le", vector_le },
    { "__ge", vector_ge },
    { "__add", vector_add },
    { "__sub", vector_sub },
    { "__mul", vector_mul },
    { "__div", vector_div },
    { "abs", vector_abs },
    { "abs2", vector_abs2 },
    { "normalized_mut", vector_normalized_mut },
    { "normalized2_mut", vector_normalized2_mut },
    { "normalized", vector_normalized },
    { "normalized2", vector_normalized2 },
    { "map_mut", vector_map_mut },
    { "map", vector_map },
    { "__tostring", vector_tostring },
    { "__len", vector_len },
    { "__index", vector_index },
    { "__newindex", vector_newindex },
    { NULL, NULL },
};

static const struct luaL_Reg module_methods[] = {
    { "new", vector_new },
    { "hex", vector_hex },
    { "name_from_direction", vector_name_from_direction },
    { NULL, NULL },
};

// The main function to open the library.
// Lua looks for a function named "luaopen_<libraryname>".
int luaopen_vector(lua_State *L) {
    // Create the metatable.
    luaL_newmetatable(L, "vector_metatable");

    // Set the __index metamethod to the table of methods itself.
    // This is how methods are looked up.
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    // Register the methods in the metatable.
    luaL_register(L, NULL, vector_methods);

    // Create the library table and register the functions.
    lua_newtable(L);
    luaL_register(L, NULL, module_methods);

    vector *zero = vector_allocate(L);
    zero->len = 2;
    zero->items[0] = 0;
    zero->items[1] = 0;
    lua_setfield(L, -2, "zero");

    vector *one = vector_allocate(L);
    one->len = 2;
    one->items[0] = 1;
    one->items[1] = 1;
    lua_setfield(L, -2, "one");

    vector *white = vector_allocate(L);
    white->len = 3;
    white->items[0] = 1;
    white->items[1] = 1;
    white->items[2] = 1;
    lua_setfield(L, -2, "white");

    vector *black = vector_allocate(L);
    black->len = 3;
    black->items[0] = 0;
    black->items[1] = 0;
    black->items[2] = 0;
    lua_setfield(L, -2, "black");

    lua_createtable(L, 4, 0);  // .extended_directions
    lua_createtable(L, 4, 0);  // .directions

    vector *up = vector_allocate(L);
    up->len = 2;
    up->items[0] = 0;
    up->items[1] = -1;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -6, "up");
    lua_rawseti(L, -4, 1);
    lua_rawseti(L, -2, 1);

    vector *down = vector_allocate(L);
    down->len = 2;
    down->items[0] = 0;
    down->items[1] = 1;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -6, "down");
    lua_rawseti(L, -4, 2);
    lua_rawseti(L, -2, 2);

    vector *left = vector_allocate(L);
    left->len = 2;
    left->items[0] = -1;
    left->items[1] = 0;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -6, "left");
    lua_rawseti(L, -4, 3);
    lua_rawseti(L, -2, 3);

    vector *right = vector_allocate(L);
    right->len = 2;
    right->items[0] = 1;
    right->items[1] = 0;
    lua_pushvalue(L, -1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -6, "right");
    lua_rawseti(L, -4, 4);
    lua_rawseti(L, -2, 4);

    lua_setfield(L, -3, "directions");

    vector *ul = vector_allocate(L);
    ul->len = 2;
    ul->items[0] = -1;
    ul->items[1] = -1;
    lua_rawseti(L, -2, 5);

    vector *ur = vector_allocate(L);
    ur->len = 2;
    ur->items[0] = 1;
    ur->items[1] = -1;
    lua_rawseti(L, -2, 6);

    vector *dl = vector_allocate(L);
    dl->len = 2;
    dl->items[0] = -1;
    dl->items[1] = 1;
    lua_rawseti(L, -2, 7);

    vector *dr = vector_allocate(L);
    dr->len = 2;
    dr->items[0] = 1;
    dr->items[1] = 1;
    lua_rawseti(L, -2, 8);

    lua_setfield(L, -2, "extended_directions");

    return 1;
}
