#include "luaPreset.h"

int luaopen_preset(lua_State *L)
{
    luaL_newlib(L, preset_functions);
    return 1;
}

void preset_onLoad(void)
{
    const char *function = "onLoad";

    luaLE_getModuleFunction("preset", function);

    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            System::logger.write("error running function '%s': %s",
                                 function,
                                 lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
    luaLE_postFunctionCleanUp(L);
}

void preset_onExit(void)
{
    const char *function = "onExit";

    luaLE_getModuleFunction("preset", function);

    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            System::logger.write("error running function '%s': %s",
                                 function,
                                 lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
    luaLE_postFunctionCleanUp(L);
}
