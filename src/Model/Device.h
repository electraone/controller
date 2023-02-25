#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "Response.h"
#include "Message.h"
#include "core_pins.h"
#include "MidiOutput.h"

class Device : public MidiOutput
{
public:
    Device() : MidiOutput(MidiInterface::Type::MidiAll, 0, 0, 0), id(0)
    {
        *name = '\0';
    }

    Device(uint8_t newId,
           const char *newName,
           uint8_t newPort,
           uint8_t newChannel,
           uint16_t newRate)
        : MidiOutput(MidiInterface::Type::MidiAll,
                     newPort,
                     newChannel,
                     newRate),
          id(newId)
    {
        setName(newName);
    }

    virtual ~Device() = default;

    bool isValid(void) const
    {
        return (id != 0);
    }

    uint8_t getId(void) const
    {
        return (id);
    }

    void setName(const char *newName)
    {
        if (newName) {
            copyString(name, newName, MaxNameLength);
        } else {
            *name = '\0';
        }
    }

    const char *getName(void) const
    {
        return (name);
    }

    uint8_t getResponseIndex(uint8_t id) const
    {
        uint8_t index = 0;

        for (auto &response : responses) {
            if (response.getId() == id) {
                return (index);
            }
            index++;
        }
        return (0);
    }

    void print(void) const
    {
        System::logger.write(ERROR, "id: %d", getId());
        System::logger.write(ERROR, "name: %s", getName());
        System::logger.write(ERROR, "port: %d", getPort());
        System::logger.write(ERROR, "channel: %d", getChannel());
        System::logger.write(ERROR, "rate: %d", getRate());
        System::logger.write(ERROR, "requests: %d", requests.size());
        System::logger.write(ERROR, "responses: %d", responses.size());
        System::logger.write(ERROR, "messages: %d", messages.size());
    }

private:
    static constexpr uint8_t MaxNameLength = 20;

    struct {
        uint8_t id : 6;
    };
    char name[MaxNameLength + 1];

public:
    std::vector<std::vector<uint8_t>> requests;
    std::vector<Response> responses;
    Messages messages;
};

typedef std::map<uint8_t, Device> Devices;
