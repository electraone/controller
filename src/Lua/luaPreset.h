#pragma once

#include "luaCommon.h"

int luaopen_preset(lua_State *L);

void preset_onLoad(void);
void preset_onExit(void);

static const luaL_Reg preset_functions[] = { { NULL, NULL } };
