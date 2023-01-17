#pragma once

#include "luaCommon.h"

int events_subscribe(lua_State *L);
int events_setPort(lua_State *L);

void events_onPageChange(uint8_t newPageId, uint8_t oldPageId);
void events_onPotTouch(uint8_t potId, uint16_t controlId, bool touched);

int luaopen_events(lua_State *L);

static const luaL_Reg events_functions[] = { { "subscribe", events_subscribe },
                                             { "setPort", events_setPort },
                                             { NULL, NULL } };
