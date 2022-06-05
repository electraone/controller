#pragma once

#include "luaHooks.h"
#include "Device.h"
#include "luaWindow.h"
#include "luaPatch.h"
#include "luaParameterMap.h"
#include "luaLE.h"

void loadLuaLibs();
void loadGlobalVariables(lua_State *L);

// table composers
void luaLE_pushDevice(Device &device);

// validators
bool luaLE_checkBoolean(lua_State *L, int idx);
int luaLE_checkDeviceId(lua_State *L, int idx);
int luaLE_checkParameterType(lua_State *L, int idx);
int luaLE_checkParameterNumber(lua_State *L, int idx);
int luaLE_checkMidiValue(lua_State *L, int idx);
int luaLE_checkControlId(lua_State *L, int idx);
int luaLE_checkInterface(lua_State *L, int idx);
int luaLE_checkPort(lua_State *L, int idx);
int luaLE_checkChannel(lua_State *L, int idx);
int luaLE_checkPageId(lua_State *L, int idx);
int luaLE_checkControlSetId(lua_State *L, int idx);
int luaLE_checkGroupId(lua_State *L, int idx);
int luaLE_checkValueIndex(lua_State *L, int idx);
