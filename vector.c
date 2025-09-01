#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Define the C structure for a 2D vector.
typedef struct {
    double x;
    double y;
} vector;

// Forward declaration of the add_mut function.
static int vector_add_mut(lua_State *L);

// A helper function to check for and retrieve a vector userdata from the Lua stack.
static vector* check_vector(lua_State *L, int index) {
    void *ud = luaL_checkudata(L, index, "vector_metatable");
    return (vector*)ud;
}

// Lua-callable function to create a new vector: vector.new(x, y)
static int vector_new(lua_State *L) {
    // Check for the correct number of arguments.
    int n = lua_gettop(L);
    if (n != 2) {
        return luaL_error(L, "Got %d arguments, expected 2 (x, y)", n);
    }

    // Get the x and y components from the arguments.
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);

    // Create a new userdata of the size of our vector struct.
    vector *v = (vector*)lua_newuserdata(L, sizeof(vector));

    // Set the metatable for the new userdata.
    luaL_getmetatable(L, "vector_metatable");
    lua_setmetatable(L, -2);

    // Initialize the vector's components.
    v->x = x;
    v->y = y;

    // Return the new userdata.
    return 1;
}

// The add_mut method: vec:add_mut(other_vec)
static int vector_add_mut(lua_State *L) {
    // The first argument is the vector itself (the 'self').
    vector *a = check_vector(L, 1);
    // The second argument is the other vector to add.
    vector *b = check_vector(L, 2);

    // Mutate the first vector by adding the second.
    a->x += b->x;
    a->y += b->y;

    // Return the modified vector.
    return 1;
}

// A function to convert the vector to a string for printing.
static int vector_tostring(lua_State *L) {
    vector *v = check_vector(L, 1);
    lua_pushfstring(L, "vector(%f, %f)", v->x, v->y);
    return 1;
}

// Array of functions to be registered as methods for vector objects.
static const struct luaL_Reg vector_methods[] = {
    { "add_mut", vector_add_mut },
    { "__tostring", vector_tostring },
    { NULL, NULL }
};

// Array of functions to be registered in the library table.
static const struct luaL_Reg vector_lib[] = {
    { "new", vector_new },
    { NULL, NULL }
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
    luaL_register(L, NULL, vector_lib);

    return 1;
}
