#include "luaInfo.h"
#include "MainDelegate.h"

extern MainDelegate *luaDelegate;

int luaopen_info(lua_State *L)
{
    luaL_newlib(L, info_functions);
    return 1;
}

int info_setText(lua_State *L)
{
    lua_settop(L, 1);
    const char *text = luaL_checkstring(L, 1);

    luaDelegate->setInfoText(text);

    return (0);
}
