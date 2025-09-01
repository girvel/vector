#include <lua.h>
#include <lauxlib.h>
#include <lua5.3/lauxlib.h>
#include <lualib.h>
#include <string.h>

#define MAX_LEN 4

typedef struct {
    int len;
    double items[MAX_LEN];
} vector;

// static int vector_add_mut(lua_State *L);

static vector* check_vector(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, "vector_metatable");
    return (vector*)ud;
}

static int vector_new(lua_State *L) {
    int n = lua_gettop(L);

    if (n > MAX_LEN) {
        return luaL_error(L, "Got %d arguments, expected %d (x, y)", n, MAX_LEN);
    }

    vector *v = (vector*)lua_newuserdata(L, sizeof(vector));
    luaL_getmetatable(L, "vector_metatable");
    lua_setmetatable(L, -2);

    v->len = n;
    for (int i = 0; i < n; i++) {
        v->items[i] = luaL_checknumber(L, i + 1);
    }

    return 1;
}

// static int vector_add_mut(lua_State *L) {
//     // The first argument is the vector itself (the 'self').
//     vector *a = check_vector(L, 1);
//     // The second argument is the other vector to add.
//     vector *b = check_vector(L, 2);
// 
//     // Mutate the first vector by adding the second.
//     a->x += b->x;
//     a->y += b->y;
// 
//     // Return the modified vector.
//     return 1;
// }

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

static inline int check_index(lua_State *L, int argument_i) {
    const char *key = luaL_checkstring(L, argument_i);
    if (strcmp(key, "x") == 0) return 0;
    if (strcmp(key, "y") == 0) return 1;
    if (strcmp(key, "z") == 0) return 2;
    if (strcmp(key, "w") == 0) return 3;
    if (strcmp(key, "r") == 0) return 0;
    if (strcmp(key, "g") == 0) return 1;
    if (strcmp(key, "b") == 0) return 2;
    if (strcmp(key, "a") == 0) return 3;
    return -1;
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
    // { "add_mut", vector_add_mut },
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
