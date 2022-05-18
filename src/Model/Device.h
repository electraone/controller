#pragma once

#include <cstdint>
#include <vector>
#include "Response.h"
#include "Message.h"

class Device
{
public:
    Device() : id(0), port(0), channel(0), rate(0), tsLastMessage(0)
    {
        *name = '\0';
    }

    Device(uint8_t newId,
           const char *newName,
           uint8_t newPort,
           uint8_t newChannel,
           uint16_t newRate)
        : id(newId),
          port(newPort),
          channel(newChannel),
          rate(newRate),
          tsLastMessage(0)
    {
        setName(newName);
    }

    virtual ~Device() = default;

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

    void setPort(uint8_t newPort)
    {
        port = newPort;
    }

    uint8_t getPort(void) const
    {
        return (port);
    }

    void setChannel(uint8_t newChannel)
    {
        channel = newChannel;
    }

    uint8_t getChannel(void) const
    {
        return (channel);
    }

    uint16_t getRate(void) const
    {
        return (rate);
    }

    void setTsLastMessage(void)
    {
        tsLastMessage = millis();
    }

    bool isReady(void)
    {
        return (((millis() - tsLastMessage) > rate) ? true : false);
    }

    uint8_t getResponseIndex(uint8_t id) const
    {
        uint8_t index = 0;

        for (auto &response : responses) {
            if (response.id == id) {
                return (index);
            }
            index++;
        }
        return (0);
    }

    void print(void) const
    {
        logMessage("id: %d", getId());
        logMessage("name: %s", getName());
        logMessage("port: %d", getPort());
        logMessage("channel: %d", getChannel());
        logMessage("rate: %d", getRate());
        logMessage("requests: %d", requests.size());
        logMessage("responses: %d", responses.size());
        logMessage("postMessages: %d", postMessages.size());
    }

private:
    static constexpr uint8_t MaxNameLength = 20;

    uint8_t id;
    char name[MaxNameLength + 1];
    uint8_t port;
    uint8_t channel;
    uint16_t rate;
    uint32_t tsLastMessage;

public:
    std::vector<std::vector<uint8_t>> requests;
    std::vector<Response> responses;
    std::vector<Message> postMessages;
};

typedef std::map<uint8_t, Device> Devices;
