#pragma once

#include <ArduinoJson.h>
#include <functional>
#include "Control.h"
#include "LocalFile.h"
#include "Origin.h"
#include "Event.h"

#include "System.h"

class ParameterMapWindow;

struct LookupEntry {
    LookupEntry(uint32_t newHash, uint16_t newMidiValue)
        : hash(newHash), midiValue(newMidiValue), dirty(false){};

    uint32_t hash;
    uint16_t midiValue;
    bool dirty;
    std::vector<ControlValue *> messageDestination;
};

/** Message hash function.
 *  creates a hash value for given message attributes. This is used for lookups
 *  in ParameterMap unordered_map.
 */
uint32_t calculateHash(uint8_t deviceId,
                       Message::Type type,
                       uint16_t parameterNumber);

uint8_t getDeviceId(uint32_t hash);
uint8_t getType(uint32_t hash);
uint16_t getParameterNumber(uint32_t hash);

class ParameterMap
{
public:
    ParameterMap();
    void setProjectId(const char *newProjectId);
    void setAppSandbox(const char *newAppSandbox);

    LookupEntry *findAndCache(uint32_t hash);
    LookupEntry *getOrCreate(uint8_t deviceId,
                             Message::Type type,
                             uint16_t parameterNumber);
    LookupEntry *
        get(uint8_t deviceId, Message::Type type, uint16_t parameterNumber);
    uint16_t getValue(uint8_t deviceId,
                      Message::Type type,
                      uint16_t parameterNumber);
    LookupEntry *setValue(uint8_t deviceId,
                          Message::Type type,
                          uint16_t parameterNumber,
                          uint16_t midiValue,
                          Origin origin);
    LookupEntry *applyToValue(uint8_t deviceId,
                              Message::Type type,
                              uint16_t parameterNumber,
                              uint16_t midiValueFragment,
                              Origin origin);
    void resetDeviceValues(uint8_t deviceId);
    void reset(void);
    void print(void);

    void keep(void);
    void save(const char *filename);
    void serialize(const char *filename);
    void serializeMap(File &file);
    void serializeRoot(File &file);

    bool recall(void);
    void forget(void);
    bool load(const char *filename);
    bool parse(File &file);
    bool parseParameters(File &file);

    bool addWindow(ParameterMapWindow *windowToAdd);
    void removeWindow(ParameterMapWindow *windowToRemove);
    void listWindows(void);

    void repaintParameterMap(void);
    void enable(void);
    void disable(void);

    /** Event handlers
    *
    */
    std::function<void(LookupEntry *entry, Origin origin)> onChange;

private:
    void prepareMapStateFilename(char *buffer, size_t maxLength);
    std::vector<LookupEntry> entries;
    uint16_t lastRead;
    uint16_t lastWritten;
    char projectId[20 + 1];
    char appSandbox[20 + 1];

    std::vector<ParameterMapWindow *> windows;

    Task repaintParameterMapTask;
};

extern ParameterMap parameterMap;
