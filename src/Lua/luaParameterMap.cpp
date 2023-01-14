#include "luaParameterMap.h"
#include "lualibs.h"

int luaopen_parameterMap(lua_State *L)
{
    luaL_newlib(L, parameterMap_functions);
    return 1;
}

int parameterMap_resetAll(lua_State *L)
{
    parameterMap.reset();

    return (0);
}

int parameterMap_resetDevice(lua_State *L)
{
    lua_settop(L, 1);

    int deviceId = luaLE_checkDeviceId(L, -1);

    parameterMap.resetDeviceValues(deviceId);

    return (0);
}

int parameterMap_set(lua_State *L)
{
    lua_settop(L, 4);

    int deviceId = luaLE_checkDeviceId(L, -4);
    int type = luaLE_checkParameterType(L, -3);
    uint16_t parameterNumber = luaLE_checkParameterNumber(L, -2);
    uint16_t midiValue = luaLE_checkMidiValue(L, -1);

    parameterMap.setValue(
        deviceId, (Message::Type)type, parameterNumber, midiValue, Origin::lua);

    return (0);
}

int parameterMap_send(lua_State *L)
{
    lua_settop(L, 3);

    int deviceId = luaLE_checkDeviceId(L, -3);
    int type = luaLE_checkParameterType(L, -2);
    uint16_t parameterNumber = luaLE_checkParameterNumber(L, -1);

    LookupEntry *entry =
        parameterMap.get(deviceId, (Message::Type)type, parameterNumber);

    if (entry) {
        for (auto value : entry->messageDestination) {
            /*
            parameterMap.setValue(deviceId,
                                  (Message::Type)type,
                                  parameterNumber,
                                  midiValue,
                                  Origin::lua);
            dest.value->message.setEvent(Event::change);
            */
            //electraMidi.sendMessage(&dest.value->message);
        }
    }

    return (0);
}

int parameterMap_apply(lua_State *L)
{
    lua_settop(L, 4);

    int deviceId = luaLE_checkDeviceId(L, -4);
    int type = luaLE_checkParameterType(L, -3);
    uint16_t parameterNumber = luaLE_checkParameterNumber(L, -2);
    uint16_t midiValue = luaLE_checkMidiValue(L, -1);

    parameterMap.applyToValue(
        deviceId, (Message::Type)type, parameterNumber, midiValue, Origin::lua);
    return (0);
}

int parameterMap_get(lua_State *L)
{
    lua_settop(L, 3);

    int deviceId = luaLE_checkDeviceId(L, -3);
    int type = luaLE_checkParameterType(L, -2);
    uint16_t parameterNumber = luaLE_checkParameterNumber(L, -1);

    uint16_t midiValue =
        parameterMap.getValue(deviceId, (Message::Type)type, parameterNumber);

    lua_pushnumber(L, midiValue);

    return (1);
}

int parameterMap_getValues(lua_State *L)
{
    lua_settop(L, 3);

    int deviceId = luaLE_checkDeviceId(L, -3);
    int type = luaLE_checkParameterType(L, -2);
    uint16_t parameterNumber = luaLE_checkParameterNumber(L, -1);

    LookupEntry *entry =
        parameterMap.get(deviceId, (Message::Type)type, parameterNumber);

    if (entry) {
        int i = 1;

        if (entry->messageDestination.size() > 0) {
            lua_newtable(L);

            for (auto value : entry->messageDestination) {
                if (value) {
                    luaLE_pushArrayObject(L, i, "Value", value);
                    i++;
                }
            }
            return (1);
        }
    }

    return (luaL_error(L, "failed: empty parameterMap entry"));
}

int parameterMap_keep(lua_State *L)
{
    lua_settop(L, 0);
    parameterMap.keep();
    return (0);
}

int parameterMap_recall(lua_State *L)
{
    lua_settop(L, 0);
    parameterMap.recall();
    return (0);
}

int parameterMap_forget(lua_State *L)
{
    lua_settop(L, 0);
    parameterMap.forget();
    return (0);
}

void parameterMap_onChange(LookupEntry *entry, Origin origin)
{
    luaLE_getModuleFunction(L, "parameterMap", "onChange");

    if (lua_isfunction(L, -1)) {
        int i = 1;

        if (entry->messageDestination.size() > 0) {
            lua_newtable(L);

            for (auto value : entry->messageDestination) {
                if (value) {
                    luaLE_pushArrayObject(L, i, "Value", value);
                    i++;
                }
            }
        }

        lua_pushnumber(L, (uint8_t)origin);
        lua_pushnumber(L, entry->midiValue);

        if (lua_pcall(L, 3, 0, 0) != 0) {
            System::logger.write(ERROR,
                                 "error running function 'onChange': %s",
                                 lua_tostring(L, -1));
        }
    } else {
        lua_pop(L, 1);
    }
}
