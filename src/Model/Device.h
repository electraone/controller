#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "Response.h"
#include "Message.h"
#include "core_pins.h"
#include "MidiOutput.h"
#include "Data.h"

class Device : public MidiOutput
{
public:
    Device();
    Device(uint8_t newId,
           const char *newName,
           uint8_t newPort,
           uint8_t newChannel,
           uint16_t newRate);

    virtual ~Device() = default;

    bool isValid(void) const;
    uint8_t getId(void) const;
    void setName(const char *newName);
    const char *getName(void) const;
    uint8_t getResponseIndex(uint8_t id) const;
    DataBytes *registerData(JsonVariant jData);
    void print(uint8_t logLevel = LOG_TRACE) const;

private:
    static constexpr uint8_t MaxNameLength = 20;

    struct {
        uint8_t id : 6;
    };
    char name[MaxNameLength + 1];
    uint16_t lastMessageId;

public:
    std::vector<std::vector<uint8_t>> requests;
    std::vector<Response> responses;
    SysexMessages sysexMessages;
};

typedef std::map<uint8_t, Device> Devices;
