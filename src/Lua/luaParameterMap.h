#pragma once

#include "ParameterMap.h"
#include "luaLE.h"

int luaopen_parameterMap(lua_State *L);

int parameterMap_resetAll(lua_State *L);
int parameterMap_resetDevice(lua_State *L);
int parameterMap_set(lua_State *L);
int parameterMap_apply(lua_State *L);
int parameterMap_get(lua_State *L);
int parameterMap_getValues(lua_State *L);
int parameterMap_keep(lua_State *L);
int parameterMap_recall(lua_State *L);
int parameterMap_forget(lua_State *L);
int parameterMap_print(lua_State *L);

void parameterMap_onChange(LookupEntry *entry, Origin origin);

static const luaL_Reg parameterMap_functions[] = {
    { "resetAll", parameterMap_resetAll },
    { "resetDevice", parameterMap_resetDevice },
    { "set", parameterMap_set },
    { "apply", parameterMap_apply },
    { "get", parameterMap_get },
    { "getValues", parameterMap_getValues },
    { "keep", parameterMap_keep },
    { "recall", parameterMap_recall },
    { "forget", parameterMap_forget },
    { "print", parameterMap_print },
    { NULL, NULL }
};
