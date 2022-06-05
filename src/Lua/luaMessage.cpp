#include "luaMessage.h"
#include "Message.h"

static Message *getMessage(lua_State *L, uint8_t stackPosition)
{
    return (*reinterpret_cast<Message **>(
        luaL_checkudata(L, stackPosition, "Message")));
}

int message_create(lua_State *L)
{
    lua_settop(L, 1);
    int index = luaLE_checkValueIndex(L, 1);

    Message *message = nullptr; // on purpose. there is no getter

    if (message) {
        luaLE_pushObject(L, "Message", message);
        return (1);
    }
    return (luaL_error(L, "failed: message %d does not exist", index));
}

int message_delete(lua_State *L)
{
    lua_settop(L, 1);
    return (0);
}

int message_getType(lua_State *L)
{
    lua_settop(L, 1);
    int messageType = (int)ElectraMessageType::invalid;

    if (Message *message = getMessage(L, 1)) {
        messageType = (int)message->getType();
    }

    lua_pushinteger(L, messageType);
    return (1);
}

int message_getParameterNumber(lua_State *L)
{
    lua_settop(L, 1);
    int parameterNumber = 0;

    if (Message *message = getMessage(L, 1)) {
        parameterNumber = message->getParameterNumber();
    }

    lua_pushinteger(L, parameterNumber);
    return (1);
}

int message_getDeviceId(lua_State *L)
{
    lua_settop(L, 1);
    int deviceId = 0;

    if (Message *message = getMessage(L, 1)) {
        deviceId = message->getDeviceId();
    }

    lua_pushinteger(L, deviceId);
    return (1);
}

int message_getValue(lua_State *L)
{
    lua_settop(L, 1);
    int midiValue = 0;

    if (Message *message = getMessage(L, 1)) {
        midiValue = message->getValue();
    }

    lua_pushinteger(L, midiValue);
    return (1);
}

int message_setValue(lua_State *L)
{
    lua_settop(L, 2);

    Message *message = getMessage(L, 1);
    int midiValue = luaL_checkinteger(L, 2);

    if (message) {
        message->setValue(midiValue);
    }
    return (0);
}

int message_getOnValue(lua_State *L)
{
    lua_settop(L, 1);
    int midiValue = 0;

    if (Message *message = getMessage(L, 1)) {
        midiValue = message->getOnValue();
    }

    lua_pushinteger(L, midiValue);
    return (1);
}

int message_getOffValue(lua_State *L)
{
    lua_settop(L, 1);
    int midiValue = 0;

    if (Message *message = getMessage(L, 1)) {
        midiValue = message->getOffValue();
    }

    lua_pushinteger(L, midiValue);
    return (1);
}

void message_register(lua_State *L)
{
    lua_register(L, "Message", message_create);
    luaL_newmetatable(L, "Message");

    lua_pushcfunction(L, message_delete);
    lua_setfield(L, -2, "__gc");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaLE_addObjectMethods(L, message_functions);
    lua_pop(L, 1);
}
