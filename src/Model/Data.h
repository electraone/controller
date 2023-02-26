#pragma once

#include <cstdint>
#include <vector>
#include "ArduinoJson.h"

typedef std::vector<uint8_t> DataBytes;
extern std::vector<std::string> luaFunctions;

class Data
{
public:
    Data(JsonArray jData);
    virtual ~Data() = default;

    DataBytes get(void) const;
    DataBytes *getPointer(void);
    void makeSysexData(void);
    void addLeadingSysexByte(void);

    static uint8_t registerFunction(const char *functionName);

private:
    void transformParameter(JsonArray jRules);
    void transformValue(JsonArray jRules);
    void transformChecksum(JsonVariant jByte);
    void transformFunction(JsonVariant jByte);

    DataBytes dataBytes;

    static DataBytes emptyData;
};
