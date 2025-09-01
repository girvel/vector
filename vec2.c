#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Define the C structure for a 2D vector.
typedef struct {
    double x;
    double y;
} vec2;

// Forward declaration of the add_mut function.
static int vec2_add_mut(lua_State *L);

// A helper function to check for and retrieve a vec2 userdata from the Lua stack.
static vec2* check_vec2(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, "vec2_metatable");
    return (vec2*)ud;
}

// Lua-callable function to create a new vector: vec2.new(x, y)
static int vec2_new(lua_State *L) {
    // Check for the correct number of arguments.
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Got %d arguments, expected 2 (x, y)", n);
    }

    // Get the x and y components from the arguments.
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);

    // Create a new userdata of the size of our vec2 struct.
    vec2 *v = (vec2*)lua_newuserdata(L, sizeof(vec2));

    // Set the metatable for the new userdata.
    luaL_getmetatable(L, "vec2_metatable");
    lua_setmetatable(L, -2);

    // Initialize the vector's components.
    v->x = x;
    v->y = y;

    // Return the new userdata.
    return 1;
}

// The add_mut method: vec:add_mut(other_vec)
static int vec2_add_mut(lua_State *L) {
    // The first argument is the vector itself (the 'self').
    vec2 *a = check_vec2(L, 1);
    // The second argument is the other vector to add.
    vec2 *b = check_vec2(L, 2);

    // Mutate the first vector by adding the second.
    a->x += b->x;
    a->y += b->y;

    // Return the modified vector.
    return 1;
}

// A function to convert the vector to a string for printing.
static int vec2_tostring(lua_State *L) {
    vec2 *v = check_vec2(L, 1);
    lua_pushfstring(L, "vec2(%f, %f)", v->x, v->y);
    return 1;
}

// Array of functions to be registered as methods for vec2 objects.
static const struct luaL_Reg vec2_methods[] = {
    { "add_mut", vec2_add_mut },
    { "__tostring", vec2_tostring },
    { NULL, NULL }
};

// Array of functions to be registered in the library table.
static const struct luaL_Reg vec2_lib[] = {
    { "new", vec2_new },
    { NULL, NULL }
};

// The main function to open the library.
// Lua looks for a function named "luaopen_<libraryname>".
int luaopen_vec2(lua_State *L) {
    // Create the metatable.
    luaL_newmetatable(L, "vec2_metatable");

    // Set the __index metamethod to the table of methods itself.
    // This is how methods are looked up.
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    // Register the methods in the metatable.
    luaL_setfuncs(L, vec2_methods, 0);

    // Create the library table and register the functions.
    luaL_newlib(L, vec2_lib);

    return 1;
}
