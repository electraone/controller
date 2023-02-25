#include "Preset.h"
#include "Hardware.h"
#include "Colours.h"
#include "JsonTools.h"
#include "System.h"

Page Preset::pageNotFound;
Device Preset::deviceNotFound;
Control Preset::controlNotFound;
Group Preset::groupNotFound;

// \todo this is a brute way of reducing memory usage by the ControlValue
std::vector<std::string> luaFunctions;

Preset::Preset() : valid(false)
{
    reset();
}

/** Load preset from the file
 *
 */
bool Preset::load(const char *filename)
{
    File file;
    valid = false; // invalidate the preset

    System::logger.write(INFO, "Preset::load: file: filename=%s", filename);

    file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        System::logger.write(
            ERROR, "Preset::load: cannot open preset file: %s", filename);
        return (false);
    }

    file.setTimeout(5);

    if (!parse(file)) {
        System::logger.write(
            ERROR, "Preset::load: cannot parse preset: filename=%s", filename);
        file.close();
        return (false);
    }

    file.close();
    valid = true;

    return (true);
}

/** Reset preset and release all the storage
 *
 */
void Preset::reset(void)
{
    resetRoot();
    resetControls();

    groups.clear();
    devices.clear();
    luaFunctions = std::vector<std::string>({ "" });
    overlays.clear();
    pages.clear();
}

/** Reset all preset controls
 *
 */
void Preset::resetRoot(void)
{
    copyString(name, "NO NAME", MaxNameLength);
    version = 0;
    projectId[0] = '\0';
}

/** Reset all preset controls
 *
 */
void Preset::resetControls(void)
{
    for (auto &[id, control] : controls) {
        control.inputs = std::vector<Input>();
        control.values = std::vector<ControlValue>();
    }

    controls.clear();
}

/*--------------------------------------------------------------------------*/

/** Indicates if the preset is loaded and valid.
 *
 */
bool Preset::isValid(void) const
{
    return (valid);
}

/** Returns the name of the preset.
 *
 */
const char *Preset::getName(void) const
{
    return (name);
}

/** Returns the preset version.
 *
 */
uint8_t Preset::getVersion(void) const
{
    return (version);
}

/** Returns the project Identifier of the preset.
 *
 */
const char *Preset::getProjectId(void) const
{
    return (projectId);
}

/*--------------------------------------------------------------------------*/

/** Get Page by the pageId
 *
 */
Page &Preset::getPage(uint8_t pageId)
{
    try {
        return pages.at(pageId);
    } catch (std::out_of_range const &) {
        System::logger.write(
            ERROR, "getPage: page does not exist: pageId=%d", pageId);
    }

    return (Preset::pageNotFound);
}

/** Get const Page by the pageId
 *
 */
const Page &Preset::getPage(uint8_t pageId) const
{
    try {
        return pages.at(pageId);
    } catch (std::out_of_range const &) {
        System::logger.write(
            ERROR, "getPage: page does not exist: pageId=%d", pageId);
    }

    return (Preset::pageNotFound);
}

/** Get Device by the id
 *
 */
const Device &Preset::getDevice(uint8_t deviceId) const
{
    try {
        return devices.at(deviceId);
    } catch (std::out_of_range const &) {
        System::logger.write(
            ERROR, "getDevice: device does not exist: deviceId=%d", deviceId);
    }

    return (Preset::deviceNotFound);
}

/** Get the Device by the port and channel
 *
 */
const Device &Preset::getDevice(uint8_t port, uint8_t channel) const
{
    for (auto &[id, device] : devices) {
        if ((device.getPort() == port) && (device.getChannel() == channel)) {
            return (device);
        }
    }

    return (Preset::deviceNotFound);
}

/** Get overlay pointer by the id
 *
 */
Overlay *Preset::getOverlay(uint8_t overlayId)
{
    Overlay *overlay = nullptr;

    try {
        overlay = &overlays.at(overlayId);
    } catch (std::out_of_range const &) {
        overlay = nullptr;
    }

    return (overlay);
}

/** Get group by the Id
 *
 */
Group &Preset::getGroup(uint16_t groupId)
{
    try {
        return groups.at(groupId);
    } catch (std::out_of_range const &) {
        System::logger.write(
            ERROR, "getGroup: group does not exist: groupId=%d", groupId);
    }

    return (Preset::groupNotFound);
}

/** Get const group by the Id
 *
 */
const Group &Preset::getGroup(uint16_t groupId) const
{
    try {
        return groups.at(groupId);
    } catch (std::out_of_range const &) {
        System::logger.write(
            ERROR, "getGroup: group does not exist: groupId=%d", groupId);
    }

    return (Preset::groupNotFound);
}

/** Get Control by the controlId
 *
 */
Control &Preset::getControl(uint16_t controlId)
{
    try {
        return controls.at(controlId);
    } catch (std::out_of_range const &) {
        System::logger.write(ERROR,
                             "getControl: control does not exist: controlId=%d",
                             controlId);
    }

    return (Preset::controlNotFound);
}

/** Get const Control by the controlId
 *
 */
const Control &Preset::getControl(uint16_t controlId) const
{
    try {
        return controls.at(controlId);
    } catch (std::out_of_range const &) {
        System::logger.write(ERROR,
                             "getControl: control does not exist: controlId=%d",
                             controlId);
    }

    return (Preset::controlNotFound);
}

/*--------------------------------------------------------------------------*/

/** Parse individual preset objects
 *
 */
bool Preset::parse(File &file)
{
    if (!parseRoot(file)) {
        System::logger.write(ERROR, "Preset::parse: parseName failed");
        reset();
        return (false);
    }
    if (version != 2) {
        System::logger.write(
            ERROR,
            "Preset::parse: incorrect preset version number: version=%d",
            version);
        reset();
        return (false);
    }
    if (!parsePages(file)) {
        System::logger.write(ERROR, "Preset::parse: parsePages failed");
        reset();
        return (false);
    }
    if (!parseDevices(file)) {
        System::logger.write(ERROR, "Preset::parse: parseDevices failed");
        reset();
        return (false);
    }
    if (!parseOverlays(file)) {
        System::logger.write(ERROR, "Preset::parse: parseOverlays failed");
        reset();
        return (false);
    }
    if (!parseGroups(file)) {
        System::logger.write(ERROR, "Preset::parse: parseGroups failed");
        reset();
        return (false);
    }
    if (!parseControls(file)) {
        System::logger.write(ERROR, "Preset::parse: parseControls failed");
        reset();
    }
    //#ifdef DEBUG
    System::logger.write(
        INFO,
        "Preset::parse: successfully parsed preset: name=%s, version=%d",
        name,
        version);
    //#endif /* DEBUG */
    return (true);
}

/** parse root preset object
 *
 */
bool Preset::parseRoot(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parseRoot: cannot rewind the file");
        return (false);
    }

    StaticJsonDocument<256> doc;
    StaticJsonDocument<64> filter;

    filter["name"] = true;
    filter["version"] = true;
    filter["projectId"] = true;

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        System::logger.write(
            ERROR, "Preset::parseRooxt: parsing failed: %s", err.c_str());
        return (false);
    }

    const char *name = doc["name"] | "No name";
    const char *projectId = doc["projectId"] | "";
    uint8_t version = doc["version"];

    // "this" is used here on purpose
    copyString(this->name, name, MaxNameLength);
    copyString(this->projectId, projectId, MaxProjectIdLength);
    this->version = version;

#ifdef DEBUG
    System::logger.write(ERROR,
                         "Preset::parseRoot: name=%s, version=%d, projectId=%s",
                         name,
                         version,
                         projectId);
#endif /* DEBUG */

    return (true);
}

/** Parse Pages array
 *
 */
bool Preset::parsePages(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parsePages: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"pages\"", ARRAY) == false) {
        System::logger.write(ERROR,
                             "Preset::parsePages: pages array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
        System::logger.write(ERROR, "Preset::parsePages: no pages defined");
        return (true);
    }

    StaticJsonDocument<256> doc;

    do {
        auto err = deserializeJson(doc, file);

        if (err) {
            System::logger.write(
                ERROR,
                "Preset::parsePages: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject jPage = doc.as<JsonObject>();

        if (jPage) {
            Page page = parsePage(jPage);

            uint8_t pageId = page.getId();

            if ((0 < pageId) && (pageId <= MaxNumPages)) {
                pages[pageId] = page;
            }
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/** Parse devices array
 *
 */
bool Preset::parseDevices(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parseDevices: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"devices\"", ARRAY) == false) {
        System::logger.write(ERROR,
                             "Preset::parseDevices: devices array not found");
        return (false);
    }

    if (isElementEmpty(file)) {
        System::logger.write(ERROR, "Preset::parseDevices: no devices defined");
        return (true);
    }

    StaticJsonDocument<256> doc;
    StaticJsonDocument<128> filter;

    filter["id"] = true;
    filter["port"] = true;
    filter["channel"] = true;
    filter["name"] = true;
    filter["rate"] = true;

    uint8_t numDevices = 0;

    do {
        size_t startPosition = file.position();
        auto err =
            deserializeJson(doc, file, DeserializationOption::Filter(filter));
        size_t endPosition = file.position();

        if (err) {
            if (err.code() != DeserializationError::TooDeep) {
                System::logger.write(
                    ERROR,
                    "Preset::parseDevices: deserializeJson() failed: %s",
                    err.c_str());
                return (false);
            }
        }

        JsonObject jDevice = doc.as<JsonObject>();

        if (jDevice) {
            Device device = this->parseDevice(jDevice);

            size_t endOfDevicePosition = file.position();
            parsePatches(file, startPosition, endPosition, &device);
            parseMessages(file, startPosition, endPosition, &device);

            if (file.seek(endOfDevicePosition) == false) {
                System::logger.write(
                    ERROR,
                    "Preset::parseDevices: cannot rewind the end position");
                return (false);
            }

            devices[device.getId()] = device;
            numDevices++;

            if (numDevices >= MaxNumDevices) {
                System::logger.write(
                    ERROR,
                    "Preset::parseDevices: max number of devices reached");
                break;
            }
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/** Parse Overlays object
 *
 */
bool Preset::parseOverlays(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parseOverlays: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"overlays\"", ARRAY) == false) {
        System::logger.write(ERROR,
                             "Preset::parseOverlays: overlays array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
#ifdef DEBUG
        System::logger.write(ERROR,
                             "Preset::parseOverlays: no overlays defined");
#endif
        return (true);
    }

    StaticJsonDocument<64> doc;

    do {
        uint32_t startPosition = file.position();
        auto err =
            deserializeJson(doc, file, DeserializationOption::NestingLimit(1));

        if (err && (err.code() != DeserializationError::TooDeep)
            && (err.code() != DeserializationError::InvalidInput)) {
            System::logger.write(
                ERROR,
                "Preset::parseOverlays: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject jOverlay = doc.as<JsonObject>();

        if (jOverlay) {
            uint8_t id = jOverlay["id"];
            overlays[id] = Overlay(id);

            if (file.seek(startPosition) == false) {
                System::logger.write(
                    ERROR,
                    "Preset::parseOverlays: cannot rewind the start position");
                return (false);
            }

            if (!this->parseOverlayItems(file, overlays[id])) {
                System::logger.write(
                    ERROR,
                    "Preset::parseOverlays: parsing of overlay items has failed");
                return (false);
            }
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/** parseGroups
 *
 */
bool Preset::parseGroups(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parseGroups: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"groups\"", ARRAY) == false) {
        System::logger.write(ERROR,
                             "Preset::parseGroups: groups array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
#ifdef DEBUG
        System::logger.write(ERROR, "Preset::parseGroups: no groups defined");
#endif
        return (true);
    }

    StaticJsonDocument<256> doc;

    do {
        auto err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            System::logger.write(
                ERROR,
                "Preset::parseGroups: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject jGroup = doc.as<JsonObject>();

        if (jGroup) {
            Group group = parseGroup(jGroup);
            groups[group.getId()] = group;
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/** Parse array of Control objects
 *
 */
bool Preset::parseControls(File &file)
{
    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::parseControls: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"controls\"", ARRAY) == false) {
        System::logger.write(ERROR,
                             "Preset::parseControls: controls array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
        System::logger.write(ERROR,
                             "Preset::parseControls: no controls defined");
        return (true);
    }

    StaticJsonDocument<1024> doc;
    StaticJsonDocument<512> filter;

    /* filter root elements only */
    filter["id"] = true;
    filter["pageId"] = true;
    filter["controlSetId"] = true;
    filter["type"] = true;
    filter["mode"] = true;
    filter["name"] = true;
    filter["color"] = true;
    filter["variant"] = true;
    filter["visible"] = true;
    filter["bounds"] = true;

    do {
        uint32_t controlStartPosition = file.position();
        auto err =
            deserializeJson(doc, file, DeserializationOption::Filter(filter));

        if (err) {
            System::logger.write(
                ERROR,
                "Preset::parseControls: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }
        uint32_t controlEndPosition = file.position();

        /* parse the Control */
        JsonObject jControl = doc.as<JsonObject>();

        if (jControl) {
            Control control = parseControl(jControl);

            uint16_t controlId = control.getId();
            controls[controlId] = control;
            controls[controlId].values = parseValues(file,
                                                     controlStartPosition,
                                                     controlEndPosition,
                                                     &controls[controlId]);
            controls[controlId].inputs = parseInputs(file,
                                                     controlStartPosition,
                                                     controlEndPosition,
                                                     control.getType());
            pages[controls[controlId].getPageId()].setHasObjects(true);
        } else {
            System::logger.write(ERROR, "parseControls: broken control");
            break;
        }

        if (file.seek(controlEndPosition) == false) {
            System::logger.write(
                ERROR, "Preset::parseControls: cannot rewind the end position");
            return (false);
        }

    } while (file.findUntil(",", "]"));

    return (true);
}

/*--------------------------------------------------------------------------*/

/** Parse Page JSON object
 *
 */
Page Preset::parsePage(JsonObject jPage)
{
    uint8_t id = constrainPageId(jPage["id"]);
    const char *name = jPage["name"] | "No name";
    uint8_t defaultControlSetId = jPage["defaultControlSetId"] | 0;

#ifdef DEBUG
    System::logger.write(
        ERROR,
        "parsePage: page created: id=%d, name=%s, defaultControlSetId=%d",
        id,
        name,
        defaultControlSetId);
#endif /* DEBUG */

    return (Page(id, name, defaultControlSetId));
}

/*--------------------------------------------------------------------------*/

/** Parse Device JSON object
 *
 */
Device Preset::parseDevice(JsonObject jDevice)
{
    uint8_t id = constrainDeviceId(jDevice["id"]);
    uint8_t port = constrainPort(jDevice["port"]);
    uint8_t channel = constrainChannel(jDevice["channel"]);
    const char *name = jDevice["name"];
    uint16_t rate = constrainRate(jDevice["rate"]);

#ifdef DEBUG
    System::logger.write(
        ERROR,
        "parseDevice: device created: id=%d, port=%d, channel=%d, name=%s, rate=%d",
        id,
        port,
        channel,
        name,
        rate);
#endif /* DEBUG */

    return (Device(id, name, port, channel, rate));
}

/** Parse array of Patches within a file
 *
 */
void Preset::parsePatches(File &file,
                          size_t startPosition,
                          size_t endPosition,
                          Device *device)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(ERROR,
                             "Preset::parsePatches: cannot rewind the file");
        return;
    }

    if (findElement(file, "\"patch\"", ARRAY, endPosition)) {
        do {
            size_t patchStartPosition = file.position();
            parsePatch(file, patchStartPosition, endPosition, device);
        } while (file.findUntil(",", "]"));
    }
}

/*
 * parsePatch
 */
void Preset::parsePatch(File &file,
                        size_t startPosition,
                        size_t endPosition,
                        Device *device)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(ERROR,
                             "Preset::parsePatch: cannot rewind the file");
        return;
    }

    if (startPosition >= endPosition) {
        file.seek(startPosition);
        return;
    }

    StaticJsonDocument<4096> doc;
    StaticJsonDocument<JSON_OBJECT_SIZE(20)> filter;

    filter["request"] = true;
    filter["messages"] = true;

    auto err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        System::logger.write(ERROR,
                             "Preset::parsePatch: deserializeJson() failed: %s",
                             err.c_str());
        return;
    }

    parsePatch(doc.as<JsonObject>(), device);
    parseResponses(file, startPosition, endPosition, device);
}

/** Parse Patch post and request JSON objects
 *
 */
void Preset::parsePatch(JsonObject jPatchItem, Device *device)
{
    device->requests.push_back(
        parseRequest(jPatchItem["request"], device->getId()));
}

/** Parse Patch responses array
 *
 */
void Preset::parseResponses(File &file,
                            size_t startPosition,
                            size_t endPosition,
                            Device *device)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseResponses: cannot rewind the start position");
        return;
    }

    if (findElement(file, "\"responses\"", ARRAY, endPosition)) {
        do {
            size_t responseStartPosition = file.position();
            StaticJsonDocument<1024> doc;
            StaticJsonDocument<128> filter;

            filter["id"] = true;
            filter["length"] = true;
            filter["header"] = true;

            auto err = deserializeJson(
                doc, file, DeserializationOption::Filter(filter));

            if (err) {
                System::logger.write(
                    ERROR,
                    "Preset::parseResponses: deserializeJson() failed: %s",
                    err.c_str());
                return;
            }

            Response response;

            response.setId(doc["id"] | 0);
            response.headers = parseResponseHeader(doc["header"]);

#ifdef DEBUG
            System::logger.write(ERROR,
                                 "parseResponses: id=%d, length=%d",
                                 response.getId(),
                                 response.headers.size());
#endif /* DEBUG */

            response.rules =
                parseRules(file, responseStartPosition, endPosition);
            device->responses.push_back(response);
        } while (file.findUntil(",", "]"));
    }
}

/** Parse Device messages array
 *
 */
void Preset::parseMessages(File &file,
                           size_t startPosition,
                           size_t endPosition,
                           Device *device)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseMessages: cannot rewind the start position");
        return;
    }

    if (findElement(file, "\"messages\"", ARRAY, endPosition)) {
        do {
            StaticJsonDocument<1024> doc;
            StaticJsonDocument<128> filter;

            filter["id"] = true;
            filter["data"] = true;

            auto err = deserializeJson(
                doc, file, DeserializationOption::Filter(filter));

            if (err) {
                System::logger.write(
                    ERROR,
                    "Preset::parseMessages: deserializeJson() failed: %s",
                    err.c_str());
                return;
            }

            uint8_t id = doc["id"] | 0;
            std::vector<uint8_t> data =
                parseData(doc["data"], 0, Message::Type::sysex);

            if (data[0] != 0xF0) {
                data.insert(data.begin(), 0xF0);
                data.push_back(0xF7);
            }
            device->messages[id] = data;

        } while (file.findUntil(",", "]"));
    }
}

/** Parse response header JSON array
 *  input numbers are considered to be Dec notation, strings are Hex
 */
std::vector<uint8_t> Preset::parseResponseHeader(JsonArray jResponseHeader)
{
    std::vector<uint8_t> data =
        parseData(jResponseHeader, 0, Message::Type::sysex);

    if (data[0] != 0xF0) {
        data.insert(data.begin(), 0xF0);
    }

    return (data);
}

uint8_t Preset::registerFunction(const char *functionName)
{
    int8_t index = -1;

    for (uint8_t i = 0; i < luaFunctions.size(); i++) {
        if (luaFunctions[i] == functionName) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        luaFunctions.push_back(functionName);
        index = luaFunctions.size() - 1;
    }

    return (index);
}

/** Parse request data JSON array
 *
 */
std::vector<uint8_t> Preset::parseRequest(JsonArray jRequest, uint8_t deviceId)
{
    std::vector<uint8_t> data =
        parseData(jRequest, deviceId, Message::Type::sysex);

    if (data[0] != 0xF0) {
        data.insert(data.begin(), 0xF0);
        data.push_back(0xF7);
    }

    return (data);
}

/*--------------------------------------------------------------------------*/

/** Parse array of overlay items of given overlay
 *
 */
bool Preset::parseOverlayItems(File &file, Overlay &overlay)
{
    if (findElement(file, "\"items\"", ARRAY) == false) {
        System::logger.write(
            ERROR, "Preset::parseOverlayItems: items array not found");
        return (false);
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(3) + 256> doc;

    do {
        auto err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            System::logger.write(
                ERROR,
                "Preset::parseOverlayItems: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject item = doc.as<JsonObject>();

        if (item) {
            int16_t value = item["value"];
            const char *label = item["label"] | "";
            const char *bitmap = item["bitmap"];

#ifdef DEBUG
            System::logger.write(
                ERROR,
                "Preset::parseOverlayItems: value=%d, label=%s, bitmap=%s",
                value,
                label,
                bitmap);
#endif /* DEBUG */

            overlay.addItem(value, label, bitmap);
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/*--------------------------------------------------------------------------*/

/** parse Group JSON object
 * The defaultGroupId is used if the groupId is not in JSON.
 * This is here for backwards compatibility.
 *
 */
Group Preset::parseGroup(JsonObject jGroup)
{
    uint16_t groupId = jGroup["id"];
    uint8_t pageId = constrainPageId(jGroup["pageId"]);
    Rectangle bounds = parseBounds(jGroup["bounds"]);
    const char *name = jGroup["name"];
    uint32_t colour =
        Colours565::fromString(jGroup["color"].as<const char *>());
    Group::Variant variantType = Group::translateVariant(jGroup["variant"]);
    return (Group(groupId, pageId, bounds, name, colour, variantType));
}

/*--------------------------------------------------------------------------*/

/** Parse individual Control object
 *
 */
Control Preset::parseControl(JsonObject jControl)
{
    uint16_t id = jControl["id"];
    uint8_t pageId = constrainPageId(jControl["pageId"]);
    const char *name = jControl["name"];
    Rectangle bounds = parseBounds(jControl["bounds"]);
    Control::Type controlType = Control::translateType(jControl["type"]);
    Control::Mode controlMode = Control::translateControlMode(jControl["mode"]);
    uint32_t colour =
        Colours565::fromString(jControl["color"].as<const char *>());
    Control::Variant variantType =
        Control::translateVariant(jControl["variant"]);
    uint8_t controlSetId = constrainControlSetId(jControl["controlSetId"]);
    bool visible = jControl["visible"] | true;

    if ((controlType == Control::Type::Fader)
        && (controlMode == Control::Mode::Default)) {
        controlMode = Control::Mode::Bipolar;
    }
    return (Control(id,
                    pageId,
                    name,
                    bounds,
                    controlType,
                    controlMode,
                    colour,
                    controlSetId,
                    variantType,
                    visible));
}

/*--------------------------------------------------------------------------*/

/** Parse an array of Control inputs
 *
 */
std::vector<Input> Preset::parseInputs(File &file,
                                       size_t startPosition,
                                       size_t endPosition,
                                       Control::Type controlType)
{
    std::vector<Input> inputs;

    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseInputs: cannot rewind the start position");
        return (inputs);
    }
    if (findElement(file, "\"inputs\"", ARRAY, endPosition)) {
        do {
            size_t valueStartPosition = file.position();

            auto input = parseInput(file, valueStartPosition, controlType);
            inputs.push_back(input);
        } while (file.findUntil(",", "]"));
    }
    return (inputs);
}

/** Parse an individual Control input
 *
 */
Input Preset::parseInput(File &file,
                         size_t startPosition,
                         Control::Type controlType)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseInput: cannot rewind the start position");
        return (Input());
    }

    StaticJsonDocument<128> doc;
    auto err = deserializeJson(doc, file);

    if (err) {
        System::logger.write(ERROR,
                             "Preset::parseInput: deserializeJson() failed: %s",
                             err.c_str());
        return (Input());
    }

    size_t inputEndPosition = file.position();

    Input input = parseInput(controlType, doc.as<JsonObject>());

    if (file.seek(inputEndPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseInput: cannot rewind the end position");
        return (input);
    }

    return (input);
}

/** Parse Input JSON object
 *
 */
Input Preset::parseInput(Control::Type controlType, JsonObject jInput)
{
    uint8_t potId = constrainPotId(jInput["potId"]);
    uint8_t valueIndex = translateValueId(controlType, jInput["valueId"] | "");

    return (Input(valueIndex, potId));
}

/*--------------------------------------------------------------------------*/

/** Parse array of Value objects in the Control
 *
 */
std::vector<ControlValue> Preset::parseValues(File &file,
                                              size_t startPosition,
                                              size_t endPosition,
                                              Control *control)
{
    // Set the initial size of vector according to the type of Control
    std::vector<ControlValue> values(Preset::getNumValues(control->getType()));

    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseValues: cannot rewind the start position");
        return (values);
    }
    if (findElement(file, "\"values\"", ARRAY, endPosition)) {
        do {
            size_t valueStartPosition = file.position();
            ControlValue value = parseValue(file, valueStartPosition, control);
            values[value.getIndex()] = value;
        } while (file.findUntil(",", "]"));
    }

    return (values);
}

/* Parse individual Value out of the Value Array
 *
 */
ControlValue
    Preset::parseValue(File &file, size_t startPosition, Control *control)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseValue: cannot rewind the start position");
        return (ControlValue());
    }

    StaticJsonDocument<4096> doc;
    auto err = deserializeJson(doc, file);

    if (err) {
        System::logger.write(ERROR,
                             "Preset::parseValue: deserializeJson() failed: %s",
                             err.c_str());
        return (ControlValue());
    }

    size_t valueEndPosition = file.position();
    ControlValue value = parseValue(control, doc.as<JsonObject>());

    if (file.seek(valueEndPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseValue: cannot rewind to the end position");
    }
    return (value);
}

/** Parse Value JSON object
 *
 */
ControlValue Preset::parseValue(Control *control, JsonObject jValue)
{
    int16_t defaultValue = 0;
    int16_t min = 0;
    int16_t max = 0;
    const char *defaultValueText = "";
    const char *formatter = nullptr;
    const char *function = nullptr;
    uint8_t formatterIndex = 0;
    uint8_t functionIndex = 0;

    const char *valueId = jValue["id"] | "";
    uint8_t overlayId = jValue["overlayId"] | 0;

    if (!jValue["formatter"].isNull()) {
        formatter = jValue["formatter"].as<char *>();
        formatterIndex = registerFunction(formatter);
    }

    if (!jValue["function"].isNull()) {
        function = jValue["function"].as<char *>();
        functionIndex = registerFunction(function);
    }

    Control::Type controlType = control->getType();
    uint8_t valueIndex = translateValueId(controlType, valueId);

    bool minNotDefined = false;
    bool maxNotDefined = false;

    if (jValue["defaultValue"].is<int16_t>()) {
        defaultValue = jValue["defaultValue"].as<int16_t>();
    } else if (jValue["defaultValue"].is<char *>()) {
        defaultValueText = jValue["defaultValue"].as<char *>();
    }

    if (!jValue["min"].isNull()) {
        min = jValue["min"].as<int16_t>();
    } else {
        minNotDefined = true;
    }

    if (!jValue["max"].isNull()) {
        max = jValue["max"].as<int16_t>();
    } else {
        maxNotDefined = true;
    }

    Message message = parseMessage(jValue["message"], controlType);

    /* If value does not define min or max, take it from the message */
    if (minNotDefined) {
        min = message.getMidiMin();
    }

    if (maxNotDefined) {
        max = message.getMidiMax();
    }

    /*
	 * make sure defaultValue is within min/max boundaries
	 */
    if ((controlType != Control::Type::Pad)
        && (controlType != Control::Type::List)) {
        if (defaultValue > max) {
            defaultValue = max;
        } else if (defaultValue < min) {
            defaultValue = min;
        }
    } else {
        if (controlType == Control::Type::Pad && defaultValueText) {
            if (strcmp(defaultValueText, "on") == 0) {
                defaultValue = message.getOnValue();
            } else {
                defaultValue = message.getOffValue();
            }
        } else {
            if (controlType == Control::Type::Pad) {
                defaultValue = message.getOffValue();
            }
        }
    }

    Overlay *overlay = getOverlay(overlayId);

    if (controlType == Control::Type::List) {
        min = 0;
        max = 1;

        if (overlayId > 0) {
            if (overlay != nullptr) {
                max = overlay->getNumItems();
            }
        }
    } else if (controlType == Control::Type::Pad) {
        min = 0;
        max = 1;
    }

#ifdef DEBUG
    System::logger.write(
        ERROR,
        "parseValue: id=%s, index=%d, defaultValue=%d, min=%d, max=%d, overlayId=%d, formatter=%s, function=%s, overlay=%x",
        valueId,
        valueIndex,
        defaultValue,
        min,
        max,
        overlayId,
        luaFunctions[formatterIndex].c_str(),
        function,
        overlay);
#endif /* DEBUG */
    return (ControlValue(control,
                         valueId,
                         valueIndex,
                         defaultValue,
                         min,
                         max,
                         overlayId,
                         message,
                         formatterIndex,
                         functionIndex,
                         overlay));
}

/*--------------------------------------------------------------------------*/

/** Parse message JSON object
 *
 */
Message Preset::parseMessage(JsonObject jMessage, Control::Type controlType)
{
    uint8_t deviceId = jMessage["deviceId"] | 0;
    uint8_t messageId = jMessage["id"] | 0;
    const char *type = jMessage["type"];
    int16_t parameterNumber = jMessage["parameterNumber"];
    int16_t min = jMessage["min"] | 0;
    int16_t max = jMessage["max"] | 0;
    int16_t value = jMessage["value"];
    int16_t offValue = jMessage["offValue"] | MIDI_VALUE_DO_NOT_SEND;
    int16_t onValue = jMessage["onValue"] | MIDI_VALUE_DO_NOT_SEND;
    bool lsbFirst = jMessage["lsbFirst"];
    bool resetRpn = jMessage["resetRpn"];
    const char *signModeInput = jMessage["signMode"];
    uint8_t bitWidth;

    if (messageId != 0) {
        System::logger.write(ERROR, "message reference present");
    }

    Message::Type messageType = Message::translateType(type);

    std::vector<uint8_t> data =
        parseData(jMessage["data"], parameterNumber, messageType);

    if (data[0] != 0xF0) {
        data.insert(data.begin(), 0xF0);
        data.push_back(0xF7);
    }

    SignMode signMode = translateSignMode(signModeInput);

    if (!jMessage["bitWidth"].isNull()) {
        bitWidth = jMessage["bitWidth"].as<uint8_t>();
    } else {
        bitWidth = getDefaultbitWidth(messageType);
    }

    if (min < 0) {
        min = 0;
    }

    if (max > 16383) {
        max = 16383;
    }

    // this sets up the message for storing it in the ParameterMap
    if (controlType == Control::Type::Pad) {
        if (messageType == Message::Type::note) {
            min = 0;
            max = 127; // 0 for note off and 127 for note on.
        } else if (messageType == Message::Type::program) {
            min = MIDI_VALUE_DO_NOT_SEND;
            max = parameterNumber;
            parameterNumber = 0;
        } else if (messageType == Message::Type::atchannel) {
            parameterNumber = 0;
            min = offValue;
            max = onValue;
        } else if (messageType == Message::Type::start
                   || messageType == Message::Type::stop
                   || messageType == Message::Type::tune) {
            min = MIDI_VALUE_DO_NOT_SEND;
            max = 127;
            parameterNumber = 0;
        } else {
            min = offValue;
            max = onValue;
        }
    } else {
        if (messageType == Message::Type::program) {
            value = 0;
            parameterNumber = 0;
        } else if (messageType == Message::Type::atchannel) {
            parameterNumber = 0;
        } else if (messageType == Message::Type::pitchbend) {
            parameterNumber = 0;
        } else if (messageType == Message::Type::spp) {
            parameterNumber = 0;
        }
    }

    System::logger.write(
        TRACE,
        "parseMessage: device=%d, id=%d, msgType=%s (%d), parameterId=%d, "
        "min=%d, max=%d, value=%d, lsbFirst=%d, resetRpn=%d, signMode=%d, "
        "bitWidth=%d",
        deviceId,
        messageId,
        type,
        messageType,
        parameterNumber,
        (min == MIDI_VALUE_DO_NOT_SEND) ? -1 : min,
        (max == MIDI_VALUE_DO_NOT_SEND) ? -1 : max,
        value,
        lsbFirst,
        resetRpn,
        signMode,
        bitWidth);

    return (Message(deviceId,
                    messageType,
                    parameterNumber,
                    min,
                    max,
                    value,
                    data,
                    lsbFirst,
                    resetRpn,
                    signMode,
                    bitWidth));
}

/** Parse Rules array within a file
 *
 */
std::vector<Rule>
    Preset::parseRules(File &file, size_t startPosition, size_t endPosition)
{
    std::vector<Rule> rules;

    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseRules: cannot rewind to the end position");
        return (rules);
    }
    if (findElement(file, "\"rules\"", ARRAY, endPosition)) {
        if (!isElementEmpty(file)) {
            do {
                size_t valueStartPosition = file.position();
                rules.push_back(parseRule(file, valueStartPosition));
            } while (file.findUntil(",", "]"));
        } else {
            file.seek(file.position() + 1);
        }
    }
    return (rules);
}

/** ParseRule within a file
 *
 */
Rule Preset::parseRule(File &file, size_t startPosition)
{
    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseRule: cannot rewind to the end position");
        return (Rule());
    }

    const size_t capacity = 256;
    StaticJsonDocument<capacity> doc;

    auto err = deserializeJson(doc, file);

    if (err) {
        System::logger.write(ERROR,
                             "Preset::parseRule: deserializeJson() failed: %s",
                             err.c_str());
        file.seek(startPosition + 2);
        return (Rule());
    }

    size_t ruleEndPosition = file.position();
    Rule rule = parseRule(doc.as<JsonObject>());

    if (file.seek(ruleEndPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseRule: cannot rewind to the end position");
    }

    return (rule);
}

/** Parse single Rule JSON object
 *
 */
Rule Preset::parseRule(JsonObject jRule)
{
    const char *type;
    uint16_t parameterNumber;
    uint16_t byte;
    uint8_t pPos;
    uint8_t bPos;
    uint8_t size;

    // TODO: this is due to compatibility with legacy patch definition
    if (!jRule["parameterNumber"].isNull()) {
        parameterNumber = jRule["parameterNumber"].as<uint16_t>();
    } else {
        parameterNumber = jRule["id"].as<uint16_t>();
    }

    if (!jRule["parameterBitPosition"].isNull()) {
        pPos = jRule["parameterBitPosition"].as<uint8_t>();
    } else {
        pPos = jRule["pPos"].as<uint8_t>();
    }

    if (!jRule["byteBitPosition"].isNull()) {
        bPos = jRule["byteBitPosition"].as<uint8_t>();
    } else {
        bPos = jRule["bPos"].as<uint8_t>();
    }

    if (!jRule["bitWidth"].isNull()) {
        size = jRule["bitWidth"].as<uint8_t>();
    } else if (!jRule["size"].isNull()) {
        size = jRule["size"].as<uint8_t>();
    } else {
        size = 7;
    }

    type = jRule["type"].as<char *>();
    byte = jRule["byte"].as<uint16_t>();

    Message::Type messageType = Message::translateType(type);

    if ((bPos + size) > 7) {
        System::logger.write(
            ERROR,
            "parseRule: position of bits in the byte exceeds 7 bits: byteBitPosition=%d, bitWidth=%d",
            bPos,
            size);
    }

    if ((pPos + size) > 14) {
        System::logger.write(
            ERROR,
            "parseRule: position of bits in the parameter exceeds 14 bits: parameterBitPosition=%d, bitWidth=%d",
            pPos,
            size);
    }

#ifdef DEBUG
    System::logger.write(
        ERROR,
        "parseRule: rule: type=%d, parameterNumber=%d, byte=%d, parameterBitPosition=%d, byteBitPosition=%d, bitWidth=%d",
        messageType,
        parameterNumber,
        byte,
        pPos,
        bPos,
        size);
#endif /* DEBUG */

    return (Rule(messageType, parameterNumber, byte, pPos, bPos, size));
}

/** Parse JSON sysex data object
 *
 */
std::vector<uint8_t> Preset::parseData(JsonArray jData,
                                       int16_t parameterNumber,
                                       Message::Type messageType)
{
    std::vector<uint8_t> data;

    for (JsonVariant jByte : jData) {
        if (jByte.is<JsonObject>()) {
            const char *type = jByte["type"];
            if (type) {
                if (strcmp(type, "value") == 0) {
                    transformValue(data, jByte["rules"]);
                } else if (strcmp(type, "parameter") == 0) {
                    transformParameter(data, jByte["rules"]);
                } else if (strcmp(type, "checksum") == 0) {
                    transformChecksum(data, jByte);
                } else if (strcmp(type, "function") == 0) {
                    transformFunction(data, jByte);
                }
            }
        } else {
            /*
             * Add plain byte
             */
            uint8_t byte;

            // Support both Hex and Dec notation
            if (jByte.is<char *>()) {
                byte = strtol(jByte, 0, 16);
            } else {
                byte = jByte.as<uint8_t>();
            }

            // Add byte, unless it is a SysEx start or stop byte
            if ((byte != 0xF0) && (byte != 0xF7)) {
                data.push_back(byte);
            }
        }
    }

    return (data);
}

/** Transforms parameter variable to the SysEx template bytes
 *
 */
void Preset::transformParameter(std::vector<uint8_t> &data, JsonArray jRules)
{
    data.push_back(VARIABLE_PARAMETER);

    if (!jRules || (jRules.size() == 0)) {
        data.push_back(0);
        data.push_back(0);
        data.push_back(7);
    } else {
        for (JsonObject jRule : jRules) {
            uint8_t pPos = 0;
            if (!jRule["parameterBitPosition"].isNull()) {
                pPos = jRule["parameterBitPosition"].as<uint8_t>();
            } else {
                pPos = jRule["pPos"].as<uint8_t>();
            }

            uint8_t bPos = 0;
            if (!jRule["byteBitPosition"].isNull()) {
                bPos = jRule["byteBitPosition"].as<uint8_t>();
            } else {
                bPos = jRule["bPos"].as<uint8_t>();
            }

            uint8_t size = 0;
            if (!jRule["bitWidth"].isNull()) {
                size = jRule["bitWidth"].as<uint8_t>();
            } else if (!jRule["size"].isNull()) {
                size = jRule["size"].as<uint8_t>();
            } else {
                size = 7;
            }

            data.push_back(pPos);
            data.push_back(bPos);
            data.push_back(size);
        }
    }
    data.push_back(VARIABLE_END);
}

/** Transforms value variable to the SysEx template bytes
 *
 */
void Preset::transformValue(std::vector<uint8_t> &data, JsonArray jRules)
{
    data.push_back(VARIABLE_DATA);

    if (!jRules || (jRules.size() == 0)) {
        data.push_back((uint8_t)Message::Type::sysex);
        data.push_back(0);
        data.push_back(0);
        data.push_back(0);
        data.push_back(0);
        data.push_back(7);
    } else {
        for (JsonObject jRule : jRules) {
            const char *type = jRule["type"].as<char *>();

            uint16_t parameterNumber = 0;
            if (!jRule["parameterNumber"].isNull()) {
                parameterNumber = jRule["parameterNumber"].as<uint16_t>();
            } else {
                parameterNumber = jRule["id"].as<uint16_t>();
            }

            uint8_t pPos = 0;
            if (!jRule["parameterBitPosition"].isNull()) {
                pPos = jRule["parameterBitPosition"].as<uint8_t>();
            } else {
                pPos = jRule["pPos"].as<uint8_t>();
            }

            uint8_t bPos = 0;
            if (!jRule["byteBitPosition"].isNull()) {
                bPos = jRule["byteBitPosition"].as<uint8_t>();
            } else {
                bPos = jRule["bPos"].as<uint8_t>();
            }

            uint8_t size = 0;
            if (!jRule["bitWidth"].isNull()) {
                size = jRule["bitWidth"].as<uint8_t>();
            } else if (!jRule["size"].isNull()) {
                size = jRule["size"].as<uint8_t>();
            } else {
                size = 7;
            }

            Message::Type messageType = Message::translateType(type);

            data.push_back((uint8_t)messageType);
            data.push_back(parameterNumber & 0x7F);
            data.push_back(parameterNumber >> 7);
            data.push_back(pPos);
            data.push_back(bPos);
            data.push_back(size);
        }
    }

    data.push_back(VARIABLE_END);
}

/** Transforms checksum variable to the SysEx template bytes
 *
 */
void Preset::transformChecksum(std::vector<uint8_t> &data, JsonVariant jByte)
{
    data.push_back(CHECKSUM);
    uint8_t start = jByte["start"].as<uint8_t>();
    uint8_t length = jByte["length"].as<uint8_t>();
    const char *algorithm = jByte["algorithm"].as<char *>();
    ChecksumAlgorithm checksumAlgorithm = translateAlgorithm(algorithm);

    data.push_back((uint8_t)checksumAlgorithm);
    data.push_back(start);
    data.push_back(length);
}

/** Transforms checksum variable to the SysEx template bytes
 *
 */
void Preset::transformFunction(std::vector<uint8_t> &data, JsonVariant jByte)
{
    const char *functionName = jByte["name"].as<char *>();

    data.push_back(LUAFUNCTION);
    data.push_back(registerFunction(functionName));
}

/*--------------------------------------------------------------------------*/

/** Parse bounds JSON object
 *
 */
Rectangle Preset::parseBounds(JsonArray jBounds)
{
    return (Rectangle(jBounds[0].as<uint16_t>(),
                      jBounds[1].as<uint16_t>(),
                      jBounds[2].as<uint16_t>(),
                      jBounds[3].as<uint16_t>()));
}

/** Parse bounds in a file
 *
 */
Rectangle
    Preset::parseBounds(File &file, size_t startPosition, size_t endPosition)
{
    Rectangle bounds;

    if (file.seek(startPosition) == false) {
        System::logger.write(
            ERROR, "Preset::parseBounds: cannot rewind the start position");
        return (bounds);
    }

    if (findElement(file, "\"bounds\"", ELEMENT, endPosition)) {
        const size_t capacity = JSON_ARRAY_SIZE(4) + 20;
        StaticJsonDocument<capacity> doc;

        auto err = deserializeJson(doc, file);
        Rectangle bounds = parseBounds(doc.as<JsonArray>());

        if (err) {
            System::logger.write(
                ERROR,
                "Preset::parseBounds: deserializeJson() of bounds failed: %s",
                err.c_str());
            return (bounds);
        }

#ifdef DEBUG
        System::logger.write(ERROR,
                             "parseBounds: bounds=[%d %d %d %d]",
                             bounds.x,
                             bounds.y,
                             bounds.width,
                             bounds.height);
#endif /* DEBUG */
    }

    return (bounds);
}

/*--------------------------------------------------------------------------*/

bool Preset::getPresetNameFast(File &file,
                               char *presetName,
                               size_t maxPresetNameLength)
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacity> doc;
    *presetName = '\0';

    if (file.seek(0) == false) {
        System::logger.write(
            ERROR, "Preset::getPresetNameFast: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"name\"", ELEMENT, 200) == false) {
        return (false);
    }

    auto err = deserializeJson(doc, file);

    if (err) {
        System::logger.write(ERROR,
                             "Preset::getPresetNameFast: parsing failed: %s",
                             err.c_str());
        return (false);
    }

    copyString(presetName, doc.as<char *>(), maxPresetNameLength);

#ifdef DEBUG
    System::logger.write(ERROR, "Preset::getPresetNameFast: name=%s", name);
#endif /* DEBUG */

    return (true);
}

/** Get preset name out of the file
 * TODO: speed up, this is too slow
 */
void Preset::getPresetName(File &file,
                           char *presetName,
                           size_t maxPresetNameLength)
{
    if (getPresetNameFast(file, presetName, maxPresetNameLength)) {
        return;
    }

    StaticJsonDocument<256> doc;
    StaticJsonDocument<64> filter;
    *presetName = '\0';

    filter["name"] = true;

    if (file.seek(0) == false) {
        System::logger.write(ERROR,
                             "Preset::getPresetName: cannot rewind the file");
        return;
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        System::logger.write(
            ERROR, "Preset::getPresetName: parsing failed: %s", err.c_str());
        return;
    }

    const char *name = doc["name"] | "No name";
    copyString(presetName, name, maxPresetNameLength);

#ifdef DEBUG
    System::logger.write(ERROR, "Preset::getPresetName: name=%s", name);
#endif /* DEBUG */
}

/*--------------------------------------------------------------------------*/

/** Translate checksum algorithm to the Enum type
 *
 */
ChecksumAlgorithm Preset::translateAlgorithm(const char *algorithm)
{
    if (algorithm) {
        if (strcmp(algorithm, "roland") == 0) {
            return (ChecksumAlgorithm::ROLAND);
        } else if (strcmp(algorithm, "waldorf") == 0) {
            return (ChecksumAlgorithm::WALDORF);
        } else if (strcmp(algorithm, "fractal") == 0) {
            return (ChecksumAlgorithm::FRACTALAUDIO);
        }
    }

    return (ChecksumAlgorithm::UNKNOWN);
}

/** Translate ValueId to the Enum type
 *
 */
uint8_t Preset::translateValueId(Control::Type type, const char *valueId)
{
    if (!valueId) {
        return (0);
    }

    if (type == Control::Type::Adsr) {
        if (strcmp(valueId, "attack") == 0) {
            return (0);
        } else if (strcmp(valueId, "decay") == 0) {
            return (1);
        } else if (strcmp(valueId, "sustain") == 0) {
            return (2);
        } else if (strcmp(valueId, "release") == 0) {
            return (3);
        }
    }

    if (type == Control::Type::Adr) {
        if (strcmp(valueId, "attack") == 0) {
            return (0);
        } else if (strcmp(valueId, "decay") == 0) {
            return (1);
        } else if (strcmp(valueId, "release") == 0) {
            return (2);
        }
    }

    if (type == Control::Type::Dx7envelope) {
        if (strcmp(valueId, "r1") == 0) {
            return (1);
        } else if (strcmp(valueId, "r2") == 0) {
            return (3);
        } else if (strcmp(valueId, "r3") == 0) {
            return (5);
        } else if (strcmp(valueId, "r4") == 0) {
            return (7);
        } else if (strcmp(valueId, "l1") == 0) {
            return (0);
        } else if (strcmp(valueId, "l2") == 0) {
            return (2);
        } else if (strcmp(valueId, "l3") == 0) {
            return (4);
        } else if (strcmp(valueId, "l4") == 0) {
            return (6);
        }
    }

    return (0);
}

uint8_t Preset::getNumValues(Control::Type type)
{
    switch (type) {
        case (Control::Type::Adr):
            return (3);

        case (Control::Type::Adsr):
            return (4);

        case (Control::Type::Dx7envelope):
            return (8);

        case (Control::Type::Fader):
        case (Control::Type::List):
        case (Control::Type::Pad):
        default:
            return (1);
    }
}

uint8_t Preset::getDefaultbitWidth(Message::Type messageType)
{
    if (messageType == Message::Type::cc14) {
        return (14);
    } else if (messageType == Message::Type::nrpn) {
        return (14);
    }

    return (7);
}

bool Preset::getPresetProjectId(File &file,
                                char *presetProjectId,
                                size_t maxProjectIdLength)
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacity> doc;

    presetProjectId[0] = '\0';

    const char *projectId = NULL;

    if (file.seek(0) == false) {
        return (false);
    }

    if (findElement(file, "\"projectId\"", ELEMENT, 300) == false) {
        return (false);
    }

    DeserializationError err = deserializeJson(doc, file);

    if (err) {
        System::logger.write(ERROR,
                             "Preset::getPresetProjectId: parsing failed: %s",
                             err.c_str());
    }

    projectId = doc.as<char *>();
    copyString(presetProjectId, projectId, maxProjectIdLength);

    return (true);
}

void Preset::print(void) const
{
    System::logger.write(ERROR,
                         "--[Preset]-------------------------------------");
    System::logger.write(ERROR, "name: %s", getName());
    System::logger.write(ERROR, "projectId: %s", getProjectId());
    System::logger.write(ERROR, "version: %d", getVersion());
    System::logger.write(ERROR, "isvalid: %d", isValid());

    System::logger.write(ERROR,
                         "--[Pages]--------------------------------------");
    for (const auto &[id, page] : pages) {
        page.print();
    }

    System::logger.write(ERROR,
                         "--[Devices]------------------------------------");
    for (const auto &[id, device] : devices) {
        device.print();
    }

    System::logger.write(ERROR,
                         "--[Overlays]-----------------------------------");
    for (const auto &[id, overlay] : overlays) {
        overlay.print();
    }

    System::logger.write(ERROR,
                         "--[Groups]-------------------------------------");
    for (const auto &[id, group] : groups) {
        group.print();
    }

    System::logger.write(ERROR,
                         "--[Controls]-----------------------------------");
    for (const auto &[id, control] : controls) {
        control.print();
    }

    System::logger.write(ERROR,
                         "--[Lua Functions]------------------------------");
    for (const auto &function : luaFunctions) {
        System::logger.write(ERROR, "function: %s", function.c_str());
    }

    System::logger.write(ERROR,
                         "--[end]----------------------------------------");
}
