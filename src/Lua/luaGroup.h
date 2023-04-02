#pragma once

#include "luaCommon.h"

int group_create(lua_State *L);
int group_delete(lua_State *L);
int group_getId(lua_State *L);
int group_setLabel(lua_State *L);
int group_getLabel(lua_State *L);
int group_setColour(lua_State *L);
int group_getColour(lua_State *L);
int group_setVisible(lua_State *L);
int group_isVisible(lua_State *L);
int group_setBounds(lua_State *L);
int group_getBounds(lua_State *L);
int group_setSlot(lua_State *L);
int group_setHorizontalSpan(lua_State *L);
int group_setVerticalSpan(lua_State *L);
int group_setVariant(lua_State *L);
int group_print(lua_State *L);

void group_register(lua_State *L);

int luaopen_groups(lua_State *L);

static const luaL_Reg groups_functions[] = { { "get", group_create },
                                             { NULL, NULL } };

static const luaL_Reg group_functions[] = {
    { "getId", group_getId },
    { "setLabel", group_setLabel },
    { "getLabel", group_getLabel },
    { "setColor", group_setColour },
    { "getColor", group_getColour },
    { "setVisible", group_setVisible },
    { "isVisible", group_isVisible },
    { "setBounds", group_setBounds },
    { "getBounds", group_getBounds },
    { "setSlot", group_setSlot },
    { "setHorizontalSpan", group_setHorizontalSpan },
    { "setVerticalSpan", group_setVerticalSpan },
    { "setVariant", group_setVariant },
    { "print", group_print },
    { NULL, NULL }
};
