#pragma once

#include "luaCommon.h"

int luaopen_helpers(lua_State *L);

int helpers_slotToBounds(lua_State *L);
int helpers_boundsToSlot(lua_State *L);

static const luaL_Reg helpers_functions[] = {
    { "slotToBounds", helpers_slotToBounds },
    { "boundsToSlot", helpers_boundsToSlot },
    { NULL, NULL }
};
