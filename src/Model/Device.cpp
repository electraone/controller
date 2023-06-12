#include "Device.h"
#include "Data.h"

Device::Device()
    : MidiOutput(MidiInterface::Type::MidiAll, 0, 0, 0),
      id(0),
      lastMessageId(100)
{
    *name = '\0';
}

Device::Device(uint8_t newId,
               const char *newName,
               uint8_t newPort,
               uint8_t newChannel,
               uint16_t newRate)
    : MidiOutput(MidiInterface::Type::MidiAll, newPort, newChannel, newRate),
      id(newId),
      lastMessageId(1)
{
    setName(newName);
}

bool Device::isValid(void) const
{
    return (id != 0);
}

uint8_t Device::getId(void) const
{
    return (id);
}

void Device::setName(const char *newName)
{
    if (newName) {
        copyString(name, newName, MaxNameLength);
    } else {
        *name = '\0';
    }
}

const char *Device::getName(void) const
{
    return (name);
}

uint8_t Device::getResponseIndex(uint8_t id) const
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

/** Create new data or assign existing
 *
 */
std::vector<uint8_t> *Device::registerData(JsonVariant jData)
{
    std::vector<uint8_t> *registeredData;

    if (jData.is<JsonArray>()) {
        Data data(jData.as<JsonArray>());
        data.makeSysexData();
        sysexMessages[lastMessageId] = data.get();
        registeredData = &sysexMessages[lastMessageId];
        lastMessageId++;
    } else {
        uint16_t sysexDataId = jData.as<uint16_t>();
        registeredData = &(sysexMessages[sysexDataId]);
    }
    return (registeredData);
}

void Device::print(uint8_t logLevel) const
{
    System::logger.write(LOG_ERROR, "id: %d", getId());
    System::logger.write(LOG_ERROR, "name: %s", getName());
    System::logger.write(LOG_ERROR, "port: %d", getPort());
    System::logger.write(LOG_ERROR, "channel: %d", getChannel());
    System::logger.write(LOG_ERROR, "rate: %d", getRate());
    System::logger.write(LOG_ERROR, "requests: %d", requests.size());
    System::logger.write(LOG_ERROR, "responses: %d", responses.size());
    System::logger.write(LOG_ERROR, "sysex messages: %d", sysexMessages.size());
}
