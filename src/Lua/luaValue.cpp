#include "luaValue.h"
#include "Preset.h"
#include "UiDelegate.h"

extern UiDelegate *luaDelegate;

static ControlValue *getControlValue(lua_State *L, uint8_t stackPosition)
{
    return (*reinterpret_cast<ControlValue **>(
        luaL_checkudata(L, stackPosition, "ControlValue")));
}

int value_create(lua_State *L)
{
    lua_settop(L, 1);
    int index = luaLE_checkValueIndex(L, 1);

    ControlValue *value = nullptr; // on purpose. there is no getter

    if (value) {
        luaLE_pushObject(L, "ControlValue", value);
        return (1);
    }
    return (luaL_error(L, "failed: value %d does not exist", index));
}

int value_delete(lua_State *L)
{
    lua_settop(L, 1);
    return (0);
}

int value_getId(lua_State *L)
{
    lua_settop(L, 1);
    const char *valueId = "value";

    if (ControlValue *value = getControlValue(L, 1)) {
        valueId = value->getId();
    }

    lua_pushstring(L, valueId);
    return (1);
}

int value_setDefault(lua_State *L)
{
    lua_settop(L, 2);

    ControlValue *value = getControlValue(L, 1);
    int valueDefault = luaL_checkinteger(L, 2);

    if (value) {
        value->setDefault(valueDefault);
    }
    return (0);
}

int value_getDefault(lua_State *L)
{
    lua_settop(L, 1);
    int valueDefault = 0;

    if (ControlValue *value = getControlValue(L, 1)) {
        valueDefault = value->getDefault();
    }

    lua_pushinteger(L, valueDefault);
    return (1);
}

int value_setMin(lua_State *L)
{
    lua_settop(L, 2);

    ControlValue *value = getControlValue(L, 1);
    int valueMin = luaL_checkinteger(L, 2);

    if (value) {
        value->setMin(valueMin);
    }
    return (0);
}

int value_getMin(lua_State *L)
{
    lua_settop(L, 1);
    int valueMin = 0;

    if (ControlValue *value = getControlValue(L, 1)) {
        valueMin = value->getMin();
    }

    lua_pushinteger(L, valueMin);
    return (1);
}

int value_setMax(lua_State *L)
{
    lua_settop(L, 2);

    ControlValue *value = getControlValue(L, 1);
    int valueMax = luaL_checkinteger(L, 2);

    if (value) {
        value->setMax(valueMax);
    }
    return (0);
}

int value_getMax(lua_State *L)
{
    lua_settop(L, 1);
    int valueMax = 0;

    if (ControlValue *value = getControlValue(L, 1)) {
        valueMax = value->getMax();
    }

    lua_pushinteger(L, valueMax);
    return (1);
}

int value_setOverlayId(lua_State *L)
{
    lua_settop(L, 2);

    ControlValue *value = getControlValue(L, 1);
    int overlayId = luaL_checkinteger(L, 2);

    if (value) {
        value->setOverlayId(overlayId);
    }
    return (0);
}

int value_getOverlayId(lua_State *L)
{
    lua_settop(L, 1);
    int overlayId = 0;

    if (ControlValue *value = getControlValue(L, 1)) {
        overlayId = value->getOverlayId();
    }

    lua_pushinteger(L, overlayId);
    return (1);
}

int value_getMessage(lua_State *L)
{
    lua_settop(L, 1);

    if (ControlValue *value = getControlValue(L, 1)) {
        Message *message = &(value->message);

        if (message) {
            luaLE_pushObject(L, "Message", message);
            return (1);
        }
    }
    return (luaL_error(L, "failed: not a valid value"));
}

int value_getControl(lua_State *L)
{
    lua_settop(L, 1);

    if (ControlValue *value = getControlValue(L, 1)) {
        Control *control = value->getControl();

        if (control) {
            luaLE_pushObject(L, "Control", control);
            return (1);
        }
    }
    return (luaL_error(L, "failed: not a valid value"));
}

void value_register(lua_State *L)
{
    lua_register(L, "ControlValue", value_create);
    luaL_newmetatable(L, "ControlValue");

    lua_pushcfunction(L, value_delete);
    lua_setfield(L, -2, "__gc");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaLE_addObjectMethods(L, value_functions);
    lua_pop(L, 1);
}