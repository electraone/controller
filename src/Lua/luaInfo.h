#pragma once

#include "luaCommon.h"

int luaopen_info(lua_State *L);

int info_setText(lua_State *L);

static const luaL_Reg info_functions[] = { { "setText", info_setText },
                                           { NULL, NULL } };
