#pragma once

#include <ArduinoJson.h>
#include <vector>
#include <map>

#include "PersistentStorage.h"
#include "Page.h"
#include "Device.h"
#include "Overlay.h"
#include "Group.h"
#include "Control.h"

#include "Rule.h"
#include "Checksum.h"

class Preset
{
public:
    Preset();
    virtual ~Preset() = default;

    bool load(const char *filename);
    void reset(void);

    bool isValid(void) const;
    const char *getName(void) const;
    uint8_t getVersion(void) const;
    const char *getProjectId(void) const;

    const Page &getPage(uint8_t pageId) const;
    const Device &getDevice(uint8_t deviceId) const;
    const Device &getDevice(uint8_t port, uint8_t channel) const;
    Overlay *getOverlay(uint8_t overlayId);
    const Group &getGroup(uint8_t groupId) const;
    Group &getGroup(uint8_t groupId);
    const Control &getControl(uint16_t controlId) const;
    Control &getControl(uint16_t controlId);

    // \todo do we need these?
    bool getPresetNameFast(File &file,
                           char *presetName,
                           size_t maxPresetNameLength);
    void
        getPresetName(File &file, char *presetName, size_t maxPresetNameLength);

    void print(void) const;

public:
    static constexpr uint8_t MaxNameLength = 20;
    static constexpr uint8_t MaxProjectIdLength = 20;

private:
    static constexpr uint8_t MaxNumPages = 12;
    static constexpr uint16_t MaxNumControls = 432;
    static constexpr uint8_t MaxNumDevices = 32;
    static constexpr uint8_t MaxNumControlSets = 3;
    static constexpr uint8_t MaxNumPots = 12;

    // Main parser
    bool parse(File &file);
    void resetRoot(void);
    void resetControls(void);

    // Root Elements
    bool parseRoot(File &file);
    bool parsePages(File &file);
    bool parseDevices(File &file);
    bool parseOverlays(File &file);
    bool parseGroups(File &file);
    bool parseControls(File &file);

    // Pages
    Page parsePage(JsonObject jPage);

    // Devices
    Device parseDevice(JsonObject jDevice);
    void parsePatches(File &file,
                      size_t startPosition,
                      size_t endPosition,
                      Device *device);
    void parsePatch(File &file,
                    size_t startPosition,
                    size_t endPosition,
                    Device *device);
    void parsePatch(JsonObject jPatchItem, Device *device);
    void parseResponses(File &file,
                        size_t startPosition,
                        size_t endPosition,
                        Device *device);
    std::vector<uint8_t> parseResponseHeader(JsonArray jResponseHeader);
    std::vector<uint8_t> parseRequest(JsonArray jRequest, uint8_t deviceId);
    std::vector<Message> parsePostMessages(JsonArray jPostPatch,
                                           uint8_t deviceId);

    // Overlays
    bool parseOverlayItems(File &file, Overlay &overlay);

    // Groups
    Group parseGroup(JsonObject jGroup, uint8_t groupId);

    // Controls
    Control parseControl(JsonObject jControl);

    // Inputs
    std::vector<Input> parseInputs(File &file,
                                   size_t startPosition,
                                   size_t endPosition,
                                   ControlType controlType);
    Input parseInput(File &file, size_t startPosition, ControlType controlType);
    Input parseInput(ControlType controlType, JsonObject jInput);

    // Values
    std::vector<ControlValue> parseValues(File &file,
                                          size_t startPosition,
                                          size_t endPosition,
                                          Control *control);
    ControlValue parseValue(File &file, size_t startPosition, Control *control);
    ControlValue parseValue(Control *control, JsonObject jValue);

    // Messages
    Message parseMessage(JsonObject jMessage);
    std::vector<Rule>
        parseRules(File &file, size_t startPosition, size_t endPosition);
    Rule parseRule(File &file, size_t startPosition);
    Rule parseRule(JsonObject jRule);
    uint8_t registerFunction(const char *functionName);
    std::vector<uint8_t> parseData(JsonArray jData,
                                   int16_t parameterNumber,
                                   ElectraMessageType electraMessageType);

    // Bounds
    Rectangle parseBounds(File &file, size_t startPosition, size_t endPosition);
    Rectangle parseBounds(JsonArray jBounds);

    // Static functions
    static ChecksumAlgorithm translateAlgorithm(const char *algorithm);
    static uint8_t translateValueId(ControlType type, const char *valueId);
    static uint8_t getNumValues(ControlType type);
    static uint8_t getDefaultbitWidth(ElectraMessageType electraMessageType);

    static uint8_t constrainPageId(uint8_t pageId)
    {
        return (constrain(pageId, 1, MaxNumPages));
    }

    static uint8_t constrainControlSetId(uint8_t controlSetId)
    {
        return (constrain(controlSetId, 1, MaxNumControlSets) - 1);
    }

    static uint8_t constrainPotId(uint8_t potId)
    {
        return (constrain(potId, 1, MaxNumPots) - 1);
    }

    static uint8_t constrainPort(uint8_t port)
    {
        return (constrain(port, 1, 2) - 1);
    }

    static uint8_t constrainChannel(uint8_t channel)
    {
        return (constrain(channel, 1, 16));
    }

    static uint16_t constrainRate(uint16_t rate)
    {
        return (constrain(rate, 10, 1000));
    }

    static uint8_t constrainDeviceId(uint8_t deviceId)
    {
        return (constrain(deviceId, 1, MaxNumDevices));
    }

    uint8_t version;
    char name[MaxNameLength + 1];
    char projectId[MaxProjectIdLength + 1];
    bool valid;

public: // Public on the purpose
    Pages pages;
    Devices devices;
    Groups groups;
    Controls controls;
    Overlays overlays;
    std::vector<std::string> luaFunctions;

    static Page pageNotFound;
    static Device deviceNotFound;
    static Control controlNotFound;
    static Group groupNotFound;
};

#define MAX_POT_ID 12
