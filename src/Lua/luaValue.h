#pragma once

#include "luaCommon.h"

int value_create(lua_State *L);
int value_delete(lua_State *L);
int value_getId(lua_State *L);
int value_setDefault(lua_State *L);
int value_getDefault(lua_State *L);
int value_setMin(lua_State *L);
int value_getMin(lua_State *L);
int value_setMax(lua_State *L);
int value_getMax(lua_State *L);
int value_getOverlayId(lua_State *L);
int value_setOverlayId(lua_State *L);
int value_getMessage(lua_State *L);
int value_getControl(lua_State *L);

void value_register(lua_State *L);

static const luaL_Reg value_functions[] = {
    { "getId", value_getId },
    { "setDefault", value_setDefault },
    { "getDefault", value_getDefault },
    { "setMin", value_setMin },
    { "getMin", value_getMin },
    { "setMax", value_setMax },
    { "getMax", value_getMax },
    { "getOverlayId", value_getOverlayId },
    { "setOverlayId", value_setOverlayId },
    { "getMessage", value_getMessage },
    { "getControl", value_getControl },
    { NULL, NULL }
};
