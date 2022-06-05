#pragma once

#include "luaCommon.h"

int control_create(lua_State *L);
int control_delete(lua_State *L);
int control_getId(lua_State *L);
int control_setVisible(lua_State *L);
int control_isVisible(lua_State *L);
int control_setName(lua_State *L);
int control_getName(lua_State *L);
int control_setColour(lua_State *L);
int control_getColour(lua_State *L);
int control_setPot(lua_State *L);
int control_setBounds(lua_State *L);
int control_getBounds(lua_State *L);
int control_setSlot(lua_State *L);
int control_getValueIds(lua_State *L);
int control_getValue(lua_State *L);
int control_getValues(lua_State *L);

void control_register(lua_State *L);

int luaopen_controls(lua_State *L);

static const luaL_Reg controls_functions[] = { { "get", control_create },
                                               { NULL, NULL } };

static const luaL_Reg control_functions[] = {
    { "getId", control_getId },
    { "setVisible", control_setVisible },
    { "isVisible", control_isVisible },
    { "setName", control_setName },
    { "getName", control_getName },
    { "setColor", control_setColour },
    { "getColor", control_getColour },
    { "setPot", control_setPot },
    { "setBounds", control_setBounds },
    { "getBounds", control_getBounds },
    { "setSlot", control_setSlot },
    { "getValueIds", control_getValueIds },
    { "getValue", control_getValue },
    { "getValues", control_getValues },
    { NULL, NULL }
};
