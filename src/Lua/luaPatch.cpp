#include "luaPatch.h"
#include "UiDelegate.h"

extern UiDelegate *luaDelegate;

int luaopen_patch(lua_State *L)
{
    luaL_newlib(L, patch_functions);
    return 1;
}

void runOnResponse(const Device &device,
                   uint8_t responseId,
                   SysexBlock &sysexBlock)
{
    luaLE_getModuleFunction(L, "patch", "onResponse");

    if (lua_isfunction(L, -1)) {
        luaLE_pushDevice(device);

        lua_pushnumber(L, responseId);
        luaLE_pushObject(L, "SysexBlock", &sysexBlock);

        if (lua_pcall(L, 3, 0, 0) != 0) {
            logMessage("error running function 'onResponse': %s",
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, "onResponse");
    }
}

void runOnRequest(const Device &device)
{
    luaLE_getModuleFunction(L, "patch", "onRequest");

    if (lua_isfunction(L, -1)) {
        luaLE_pushDevice(device);

        if (lua_pcall(L, 1, 0, 0) != 0) {
            logMessage("error running function 'onRequest': %s",
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, "onRequest");
    }
}

int patch_requestAll(lua_State *L)
{
    lua_settop(L, 0);

    luaDelegate->requestAllPatches();

    return (0);
}
