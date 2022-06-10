#pragma once

#include "luaCommon.h"

int device_create(lua_State *L);
int device_delete(lua_State *L);
int device_getByPortChannel(lua_State *L);
int device_getId(lua_State *L);
int device_setName(lua_State *L);
int device_getName(lua_State *L);
int device_setRate(lua_State *L);
int device_getRate(lua_State *L);
int device_setPort(lua_State *L);
int device_getPort(lua_State *L);
int device_setChannel(lua_State *L);
int device_getChannel(lua_State *L);

void device_register(lua_State *L);

int luaopen_devices(lua_State *L);

static const luaL_Reg devices_functions[] = { { "get", device_create },
                                              { "getByPortChannel",
                                                device_getByPortChannel },
                                              { NULL, NULL } };

static const luaL_Reg device_functions[] = {
    { "getId", device_getId },           { "setName", device_setName },
    { "getName", device_getName },       { "setRate", device_setRate },
    { "getRate", device_getRate },       { "setPort", device_setPort },
    { "getPort", device_getPort },       { "setChannel", device_setChannel },
    { "getChannel", device_getChannel }, { NULL, NULL }
};
