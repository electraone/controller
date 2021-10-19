#pragma once

#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <vector>
#include <array>

#include "ElectraMessage.h"
#include "SignMode.h"
#include "Control.h"
#include "Overlays.h"
#include "MemoryPool.h"
#include "BitmapPool.h"
#include "Value2.h"
#include "Group.h"
#include "Page.h"
#include "Device.h"
#include "Response.h"
#include "Rule.h"
#include "Input.h"
#include "Checksum.h"

class Preset
{
public:
    Preset();
    ~Preset();

    bool isValid(void);
    const char *getName(void);
    uint8_t getVersion(void);
    const char *getProjectId(void);

    Overlay *getOverlay(uint16_t id);
    Control *getControl(uint16_t id);
    Page *getPage(uint8_t pageId);
    Group *getGroup(uint8_t groupId);
    bool load(const char *filename);
    void reset(void);
    bool getPresetNameFast(File &file,
                           char *presetName,
                           size_t maxPresetNameLength);
    void
        getPresetName(File &file, char *presetName, size_t maxPresetNameLength);

public:
    static constexpr uint8_t MaxNumPages = 12;

private:
    static constexpr uint8_t MaxNameLength = 20;
    static constexpr uint16_t MaxNumControls = 432;
    static constexpr uint8_t MaxProjectIdLength = 20;
    static constexpr uint8_t MaxNumDevices = 32;

    bool parse(File &file);
    bool parseRoot(File &file);
    bool parseControls(File &file);
    Control parseControl(JsonObject jControl, File &file);
    bool parseOverlays(File &file);
    bool parseOverlayItems(File &file, Overlay *overlay);
    std::vector<Input> parseInputs(File &file,
                                   size_t startPosition,
                                   size_t endPosition,
                                   ControlType controlType);
    Input parseInput(ControlType controlType, JsonObject jInput);
    Input parseInput(File &file, size_t startPosition, ControlType controlType);
    std::vector<Value2> parseValues(File &file,
                                    size_t startPosition,
                                    size_t endPosition,
                                    Control *control);
    Value2 parseValue(Control *control, JsonObject jValue);
    Value2 parseValue(File &file, size_t startPosition, Control *control);
    Message parseMessage(JsonObject jMessage);
    bool parseGroups(File &file);
    Group parseGroup(JsonObject jGroup, uint8_t groupId);
    bool parseDevices(File &file);
    Rule parseRule(File &file, size_t startPosition);
    std::vector<Rule>
        parseRules(File &file, size_t startPosition, size_t endPosition);
    Device parseDevice(JsonObject jDevice);
    void parsePatches(File &file,
                      size_t startPosition,
                      size_t endPosition,
                      Device *device);
    void parsePatch(File &file,
                    size_t startPosition,
                    size_t endPosition,
                    Device *device);
    void parseResponses(File &file,
                        size_t startPosition,
                        size_t endPosition,
                        Device *device);
    void parsePatch(JsonObject jPatchItem, Device *device);
    std::vector<Message> parsePostMessages(JsonArray jPostPatch,
                                           uint8_t deviceId);
    std::vector<uint8_t> parseRequest(JsonArray jRequest, uint8_t deviceId);
    Rule parseRule(JsonObject jRule);
    std::vector<uint8_t> parseResponseHeader(JsonArray jResponseHeader);
    void parsePatchHeader(File &file, Device *device, uint8_t headerId);
    void parsePatchRules(File &file, Device *device, uint8_t headerId);

    Bitmap parseBitmap(const char *bitmap);
    Rectangle parseBounds(JsonArray jBounds);
    Rectangle parseBounds(File &file, size_t startPosition, size_t endPosition);
    std::vector<uint8_t> parseData(JsonArray jData,
                                   int16_t parameterNumber,
                                   ElectraMessageType electraMessageType);
    Page parsePage(JsonObject jPage);
    bool parsePages(File &file);

    static ChecksumAlgorithm translateAlgorithm(const char *algorithm);
    static uint8_t translateValueId(ControlType type, const char *valueId);
    static uint8_t getNumValues(ControlType type);
    static uint8_t getDefaultbitWidth(ElectraMessageType electraMessageType);
    static void displayStream(File &file);

    uint8_t version;
    char name[MaxNameLength + 1];
    Overlays overlays;
    char projectId[MaxProjectIdLength + 1];
    bool valid;

public:
    std::array<Page, MaxNumPages> pages;
    std::vector<Device> devices;
    std::vector<Group> groups;
    std::array<Control, MaxNumControls + 1> controls;
    std::vector<String> luaFunctions;
};

bool findElement(File &file,
                 const char *elementName,
                 ElementType elementType,
                 size_t endPosition = 0);
bool isElementEmpty(File &file);

#define MAX_POT_ID 12
