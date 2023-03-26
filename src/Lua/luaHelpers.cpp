#include "luaHelpers.h"
#include "lualibs.h"

int luaopen_helpers(lua_State *L)
{
    luaL_newlib(L, helpers_functions);
    return 1;
}

int helpers_slotToBounds(lua_State *L)
{
    lua_settop(L, 1);

    int slot = luaL_checkinteger(L, -1);

    luaL_argcheck(
        L, 1 <= slot && slot <= 36, 2, "failed: slot must be between 1 and 36");

    Rectangle bounds = controlSlotToBounds(slot);

    lua_newtable(L);
    luaLE_pushArrayInteger(L, 1, bounds.getX());
    luaLE_pushArrayInteger(L, 2, bounds.getY());
    luaLE_pushArrayInteger(L, 3, 158);
    luaLE_pushArrayInteger(L, 4, 56);

    return (1);
}

int helpers_boundsToSlot(lua_State *L)
{
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TTABLE);

    // parse type
    lua_rawgeti(L, 1, 1);
    lua_rawgeti(L, 1, 2);
    lua_rawgeti(L, 1, 3);
    lua_rawgeti(L, 1, 4);
    int x = luaL_checkinteger(L, -4);
    int y = luaL_checkinteger(L, -3);
    int width = luaL_checkinteger(L, -2);
    int height = luaL_checkinteger(L, -1);

    uint8_t slot = controlBoundsToSlot(Rectangle(x, y, width, height));

    lua_pushinteger(L, slot);
    return (1);
}
