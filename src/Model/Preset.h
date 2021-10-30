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

#include "Bitmap.h"

class Preset
{
public:
    Preset();
    ~Preset() = default;

    bool load(const char *filename);
    void reset(void);

    bool isValid(void) const;
    const char *getName(void) const;
    uint8_t getVersion(void) const;
    const char *getProjectId(void) const;

    Page *getPage(uint8_t pageId);
    Device *getDevice(uint8_t deviceId);
    Overlay *getOverlay(uint8_t overlayId);
    Group *getGroup(uint8_t groupId);
    Control *getControl(uint16_t controlId);

    // \todo do we need these?
    bool getPresetNameFast(File &file,
                           char *presetName,
                           size_t maxPresetNameLength);
    void
        getPresetName(File &file, char *presetName, size_t maxPresetNameLength);

    void print(void) const;

private:
    static constexpr uint8_t MaxNumPages = 12;
    static constexpr uint8_t MaxNameLength = 20;
    static constexpr uint16_t MaxNumControls = 432;
    static constexpr uint8_t MaxProjectIdLength = 20;
    static constexpr uint8_t MaxNumDevices = 32;

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
    std::vector<Value2> parseValues(File &file,
                                    size_t startPosition,
                                    size_t endPosition,
                                    Control *control);
    Value2 parseValue(File &file, size_t startPosition, Control *control);
    Value2 parseValue(Control *control, JsonObject jValue);

    // Messages
    Message parseMessage(JsonObject jMessage);
    std::vector<Rule>
        parseRules(File &file, size_t startPosition, size_t endPosition);
    Rule parseRule(File &file, size_t startPosition);
    Rule parseRule(JsonObject jRule);
    std::vector<uint8_t> parseData(JsonArray jData,
                                   int16_t parameterNumber,
                                   ElectraMessageType electraMessageType);

    // Bounds
    Rectangle parseBounds(File &file, size_t startPosition, size_t endPosition);
    Rectangle parseBounds(JsonArray jBounds);

    // Bitmaps
    Bitmap parseBitmap(const char *bitmap);

    // Static functions
    static ChecksumAlgorithm translateAlgorithm(const char *algorithm);
    static uint8_t translateValueId(ControlType type, const char *valueId);
    static uint8_t getNumValues(ControlType type);
    static uint8_t getDefaultbitWidth(ElectraMessageType electraMessageType);

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
    std::vector<String> luaFunctions;
};

/* functions to read JSON file */
bool findElement(File &file,
                 const char *elementName,
                 ElementType elementType,
                 size_t endPosition = 0);
bool isElementEmpty(File &file);

#define MAX_POT_ID 12
