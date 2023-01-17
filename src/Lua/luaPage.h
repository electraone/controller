#pragma once

#include "luaCommon.h"

int pages_get(lua_State *L);
int pages_getActive(lua_State *L);
int pages_display(lua_State *L);
void pages_onChange(uint8_t newPageId, uint8_t oldPageId);
int pages_setActiveControlSet(lua_State *L);
int pages_getActiveControlSet(lua_State *L);
int page_delete(lua_State *L);
int page_getId(lua_State *L);
int page_setName(lua_State *L);
int page_getName(lua_State *L);

void page_register(lua_State *L);

int luaopen_pages(lua_State *L);

static const luaL_Reg pages_functions[] = {
    { "get", pages_get },
    { "getActive", pages_getActive },
    { "display", pages_display },
    { "setActiveControlSet", pages_setActiveControlSet },
    { "getActiveControlSet", pages_getActiveControlSet },
    { NULL, NULL }
};

static const luaL_Reg page_functions[] = { { "getId", page_getId },
                                           { "getName", page_getName },
                                           { "setName", page_setName },
                                           { NULL, NULL } };
