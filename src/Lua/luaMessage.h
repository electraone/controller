#pragma once

#include "luaCommon.h"

int message_create(lua_State *L);
int message_delete(lua_State *L);
int message_getType(lua_State *L);
int message_getParameterNumber(lua_State *L);
int message_getDeviceId(lua_State *L);
int message_getValue(lua_State *L);
int message_setValue(lua_State *L);
int message_getOnValue(lua_State *L);
int message_getOffValue(lua_State *L);

void message_register(lua_State *L);

static const luaL_Reg message_functions[] = {
    { "getType", message_getType },
    { "getParameterNumber", message_getParameterNumber },
    { "getDeviceId", message_getDeviceId },
    { "getValue", message_getValue },
    { "setValue", message_setValue },
    { "getOnValue", message_getOnValue },
    { "getOffValue", message_getOffValue },
    { NULL, NULL }
};
