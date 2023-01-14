#include "luaEvents.h"
#include "MainDelegate.h"

extern MainDelegate *luaDelegate;

int luaopen_events(lua_State *L)
{
    luaL_newlib(L, events_functions);
    return 1;
}

int events_subscribe(lua_State *L)
{
    lua_settop(L, 1);
    int events = luaLE_checkEvents(L, 1);
    luaDelegate->setSubscribedEvents(events);
    return (0);
}

int events_setPort(lua_State *L)
{
    lua_settop(L, 1);
    int port = luaLE_checkPort(L, 1);
    luaDelegate->setControlPort(port);
    return (0);
}

void events_onPageChange(uint8_t oldPageId, uint8_t newPageId)
{
    const char *function = "onPageChange";
    luaLE_getModuleFunction("events", function);

    if (lua_isfunction(L, -1)) {
        lua_pushnumber(L, oldPageId);
        lua_pushnumber(L, newPageId);
        if (lua_pcall(L, 2, 0, 0) != 0) {
            System::logger.write("error running function '%s': %s",
                                 function,
                                 lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
    luaLE_postFunctionCleanUp(L);
}

void events_onPotTouch(uint8_t potId, uint16_t controlId, bool touched)
{
    const char *function = "onPotTouch";
    luaLE_getModuleFunction("events", function);

    if (lua_isfunction(L, -1)) {
        lua_pushnumber(L, potId);
        lua_pushnumber(L, controlId);
        lua_pushnumber(L, touched);
        if (lua_pcall(L, 3, 0, 0) != 0) {
            System::logger.write("error running function '%s': %s",
                                 function,
                                 lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
    luaLE_postFunctionCleanUp(L);
}
