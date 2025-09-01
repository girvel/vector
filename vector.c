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

// A function to convert the vector to a string for printing.
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
    { "__index", vector_index },
    { "__newindex", vector_newindex },
    { NULL, NULL },
};

static const struct luaL_Reg module_methods[] = {
    { "new", vector_new },
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

    return 1;
}
