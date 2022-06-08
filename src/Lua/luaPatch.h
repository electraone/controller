#pragma once

#include "luaCommon.h"
#include "SysexBlock.h"
#include "Device.h"

int luaopen_patch(lua_State *L);

void runOnResponse(const Device &device,
                   uint8_t responseId,
                   SysexBlock &sysexBlock);
void runOnRequest(const Device &device);
int patch_requestAll(lua_State *L);

static const luaL_Reg patch_functions[] = { { "requestAll", patch_requestAll },
                                            { NULL, NULL } };
