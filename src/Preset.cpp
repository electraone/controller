#include "Preset.h"
#include "Hardware.h"
#include "Colours.h"
#include "Base64.h"

MemoryPool stringPool(SDRAM_PAGE_1, SDRAM_PAGE_SIZE);
BitmapPool bitmapPool(SDRAM_PAGE_2, SDRAM_PAGE_SIZE);

Preset::Preset() : overlays(Overlays(stringPool)), valid(false)
{
    reset();
}

/** Load preset from the file
 *
 */
bool Preset::load(const char *filename)
{
    File file;

    valid = false;

    logMessage("Preset::load: file: filename=%s", filename);

    file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        logMessage("Preset::load: cannot open preset file: %s", filename);
        return (false);
    }

    file.setTimeout(5);

    if (!parse(file)) {
        logMessage("Preset::load: cannot parse preset: filename=%s", filename);
        file.close();
        return (false);
    } else {
        file.close();
    }

    valid = true;

    return (true);
}

/** Reset preset and release all the storage
 *
 */
void Preset::reset(void)
{
    copyString(name, "NO NAME", MaxNameLength);
    version = 0;
    projectId[0] = '\0';

    for (auto &control : controls) {
        control.inputs = std::vector<Input>();
        control.values = std::vector<Value2>();
    }

    controls.fill(Control());
    groups = std::vector<Group>();
    devices = std::vector<Device>();
    luaFunctions = std::vector<String>();
    bitmapPool.clear();
    overlays.reset();
    pages.fill(Page());
}

/*--------------------------------------------------------------------------*/

/** Indicates if the preset is loaded and valid.
 *
 */
bool Preset::isValid(void)
{
    return (valid);
}

/** Returns the name of the preset.
 *
 */
const char *Preset::getName(void)
{
    return (name);
}

/** Returns the preset version.
 *
 */
uint8_t Preset::getVersion(void)
{
    return (version);
}

/** Returns the project Identifier of the preset.
 *
 */
const char *Preset::getProjectId(void)
{
    return (projectId);
}

/*--------------------------------------------------------------------------*/

/** Get Page by the pageId
 *
 */
Page *Preset::getPage(uint8_t pageId)
{
    pageId--;

    if ((0 <= pageId) && (pageId <= 11)) {
        for (auto &page : pages) {
            if (page.getId() == pageId) {
                return (&page);
            }
        }
    }
    return (nullptr);
}

/** Get overlay pointer by the id
 *
 */
Overlay *Preset::getOverlay(uint16_t id)
{
    return (overlays.get(id));
}

/** Get Page by the pageId
 *
 */
Group *Preset::getGroup(uint8_t groupId)
{
    if ((0 <= groupId) && (groupId < groups.size())) {
        for (auto &group : groups) {
            if (group.getId() == groupId) {
                return (&group);
            }
        }
    }
    return (nullptr);
}

/** Get Control by id
 *
 */
Control *Preset::getControl(uint16_t id)
{
    if ((1 <= id) && (id <= 432)) {
        if (controls[id].getType() != ControlType::none) {
            return (&controls[id]);
        }
    }

    return (nullptr);
}

/*--------------------------------------------------------------------------*/

bool Preset::getPresetNameFast(File &file,
                               char *presetName,
                               size_t maxPresetNameLength)
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacity> doc;

    const char *name = NULL;

    if (file.seek(0) == false) {
        return (false);
    }

    if (findElement(file, "\"name\"", ELEMENT, 100) == false) {
        return (false);
    }

    DeserializationError err = deserializeJson(doc, file);

    if (err) {
        logMessage("Preset::getPresetName: parsing failed: %s", err.c_str());
    }

    name = doc.as<char *>();
    copyString(presetName, name, maxPresetNameLength);
    logMessage("Preset::getPresetName: name=%s", name);

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

    const size_t capacity = 128;
    const size_t capacityFilter = 64;
    StaticJsonDocument<capacity> doc;
    StaticJsonDocument<capacityFilter> filter;

    *presetName = '\0';

    filter["name"] = true;

    const char *name = NULL;

    if (file.seek(0) == false) {
        logMessage("Preset::getPresetName: cannot rewind the file");
        return;
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Preset::getPresetName: parsing failed: %s", err.c_str());
        return;
    }

    name = doc["name"].as<char *>();

    if (!name) {
        logMessage("Preset::getPresetName: name not present");
        return;
    }

    copyString(
        presetName, (*name == '\0') ? "NO NAME" : name, maxPresetNameLength);
    logMessage("Preset::getPresetName: name=%s", name);
}

/*--------------------------------------------------------------------------*/

/** Parse individual preset objects
 *
 */
bool Preset::parse(File &file)
{
    if (!parseRoot(file)) {
        logMessage("Preset::parse: parseName failed");
        reset();
        return (false);
    }

    if (version != 2) {
        logMessage("Preset::parse: incorrect preset version number: version=%d",
                   version);
        reset();
        return (false);
    }

    if (!parsePages(file)) {
        logMessage("Preset::parse: parsePages failed");
        reset();
        return (false);
    }

    if (!parseDevices(file)) {
        logMessage("Preset::parse: parseDevices failed");
        reset();
        return (false);
    }

    if (!parseOverlays(file)) {
        logMessage("Preset::parse: parseOverlays failed");
        reset();
        return (false);
    }

    if (!parseControls(file)) {
        logMessage("Preset::parse: parseControls failed");
        reset();
        return (false);
    }

    if (!parseGroups(file)) {
        logMessage("Preset::parse: parseGroups failed");
        reset();
        return (false);
    }

    logMessage("Preset::parse: successfully loaded preset: name=%s, version=%d",
               this->name,
               this->version);

    return (true);
}

/** parse root preset object
 *
 */
bool Preset::parseRoot(File &file)
{
    const size_t capacity = 256;
    const size_t capacityFilter = 64;
    StaticJsonDocument<capacity> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["name"] = true;
    filter["version"] = true;
    filter["projectId"] = true;

    const char *name = NULL;
    const char *projectId = NULL;
    uint8_t version;

    if (file.seek(0) == false) {
        logMessage("Preset::parseRoot: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Preset::parseRoot: parsing failed: %s", err.c_str());
        return (false);
    }

    name = doc["name"].as<char *>();
    version = doc["version"].as<uint8_t>();
    projectId = doc["projectId"].as<char *>();

    if (!name) {
        logMessage("Preset::parseRoot: name not present");
        return (false);
    }

    copyString(this->name, (*name == '\0') ? "NO NAME" : name, MaxNameLength);
    this->version = version;

    if (projectId) {
        copyString(this->projectId, projectId, MaxProjectIdLength);
    } else {
        this->projectId[0] = '\0';
    }

    logMessage("Preset::parseRoot: name=%s, version=%d, projectId=%s",
               name,
               version,
               projectId);
    return (true);
}

/** Parse Pages array
 *
 */
bool Preset::parsePages(File &file)
{
    const size_t capacity = 64;
    StaticJsonDocument<capacity> doc;

    if (file.seek(0) == false) {
        logMessage("Preset::parsePages: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"pages\"", ARRAY) == false) {
        logMessage("Preset::parsePages: pages array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
        logMessage("Preset::parsePages: no pages defined");
        return (true);
    }

    do {
        DeserializationError err = deserializeJson(doc, file);

        if (err) {
            logMessage("Preset::parsePages: deserializeJson() failed: %s",
                       err.c_str());
            return (false);
        }

        JsonObject jPage = doc.as<JsonObject>();

        if (jPage) {
            Page page = parsePage(jPage);

            uint8_t pageId = page.getId();

            if ((pageId >= 0) && (pageId < MaxNumPages)) {
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
    const size_t capacity = 256;
    const size_t capacityFilter = 128;
    StaticJsonDocument<capacity> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["id"] = true;
    filter["port"] = true;
    filter["channel"] = true;
    filter["name"] = true;
    filter["rate"] = true;

    uint8_t numDevices = 0;

    if (file.seek(0) == false) {
        logMessage("Preset::parseDevices: cannot rewind the file");
        return (false);
    }
    if (findElement(file, "\"devices\"", ARRAY) == false) {
        logMessage("Preset::parseDevices: devices array not found");
        return (false);
    }

    if (isElementEmpty(file)) {
        logMessage("Preset::parseDevices: no devices defined");
        return (true);
    }

    do {
        size_t startPosition = file.position();
        DeserializationError err =
            deserializeJson(doc, file, DeserializationOption::Filter(filter));
        size_t endPosition = file.position();
        if (err) {
            if (err.code() != DeserializationError::TooDeep) {
                logMessage("Preset::parseDevices: deserializeJson() failed: %s",
                           err.c_str());
                return (false);
            }
        }

        JsonObject jDevice = doc.as<JsonObject>();
        if (jDevice) {
            Device device = this->parseDevice(jDevice);
            size_t endOfDevicePosition = file.position();
            parsePatches(file, startPosition, endPosition, &device);
            file.seek(endOfDevicePosition);

            devices.push_back(device);
            numDevices++;

            if (numDevices >= MaxNumDevices) {
                logMessage(
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
    const size_t capacity = 64;
    StaticJsonDocument<capacity> doc;

    if (file.seek(0) == false) {
        logMessage("Preset::parseOverlays: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"overlays\"", ARRAY) == false) {
        logMessage("Preset::parseOverlays: overlays array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
        logMessage("Preset::parseOverlays: no overlays defined");
        return (true);
    }

    // always create an empty overlay with id 0
    // It resolves situations when the overlay definition is missing
    overlays.create(0);

    do {
        uint32_t pos = file.position();
        DeserializationError err =
            deserializeJson(doc, file, DeserializationOption::NestingLimit(1));

        if (err && (err.code() != DeserializationError::TooDeep)
            && (err.code() != DeserializationError::InvalidInput)) {
            logMessage("Preset::parseOverlays: deserializeJson() failed: %s",
                       err.c_str());
            return (false);
        }

        /* parse Overlay object */
        JsonObject jOverlay = doc.as<JsonObject>();

        if (jOverlay) {
            uint8_t id = jOverlay["id"].as<uint8_t>();
            //logMessage ("Preset::parseOverlays: id=%d", id);

            /* register the overlayId in the Overlay storage */
            Overlay *overlay = overlays.create(id);

            file.seek(pos);

            if (!this->parseOverlayItems(file, overlay)) {
                logMessage(
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
    uint8_t groupId = 0;
    const size_t capacity = 256;
    StaticJsonDocument<capacity> doc;

    if (file.seek(0) == false) {
        logMessage("Preset::parseGroups: cannot rewind the file");
        return (false);
    }
    if (findElement(file, "\"groups\"", ARRAY) == false) {
        logMessage("Preset::parseGroups: groups array not found");
        return (true);
    }

    if (isElementEmpty(file)) {
        logMessage("Preset::parseGroups: no groups defined");
        return (true);
    }

    do {
        DeserializationError err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            logMessage("Preset::parseGroups: deserializeJson() failed: %s",
                       err.c_str());
            return (false);
        }

        JsonObject jGroup = doc.as<JsonObject>();

        if (jGroup) {
            groups.push_back(parseGroup(jGroup, groupId));
        } else {
            break;
        }

        groupId++;
    } while (file.findUntil(",", "]"));

    return (true);
}

/** Parse array of Control objects
 *
 */
bool Preset::parseControls(File &file)
{
    const size_t capacity = 1024;
    const size_t capacityFilter = 256;
    StaticJsonDocument<capacity> doc;
    StaticJsonDocument<capacityFilter> filter;

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

    if (file.seek(0) == false) {
        logMessage("Preset::parseControls: cannot rewind the file");
        return (false);
    }

    if (findElement(file, "\"controls\"", ARRAY) == false) {
        logMessage("Preset::parseControls: controls array not found");
        return (false);
    }

    if (isElementEmpty(file)) {
        logMessage("Preset::parseControls: no controls defined");
        return (true);
    }

    do {
        uint32_t controlStartPosition = file.position();
        DeserializationError err =
            deserializeJson(doc, file, DeserializationOption::Filter(filter));

        if (err) {
            logMessage("Preset::parseControls: deserializeJson() failed: %s",
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

            if ((controlId >= 1) && (controlId <= MaxNumControls)) {
                controls[controlId].setBounds(parseBounds(
                    file, controlStartPosition, controlEndPosition));
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
                logMessage(
                    "Preset::parseControls: control cannot be inserted: controlId=%d",
                    controlId);
            }
        } else {
            break;
        }
        file.seek(controlEndPosition);

    } while (file.findUntil(",", "]"));

    return (true);
}

/*--------------------------------------------------------------------------*/

/** Parse Page JSON object
 *
 */
Page Preset::parsePage(JsonObject jPage)
{
    uint8_t id;
    const char *name;

    id = jPage["id"].as<int>();
    name = jPage["name"].as<char *>();

    logMessage("parsePage: page created: id=%d name=%s", id, name);

    if (id > 0) {
        id--;
    }

    return (Page(id, name));
}

/*--------------------------------------------------------------------------*/

/** Parse Device JSON object
 *
 */
Device Preset::parseDevice(JsonObject jDevice)
{
    uint8_t id = jDevice["id"];
    uint8_t port = jDevice["port"];
    uint8_t channel = jDevice["channel"];
    uint16_t rate = jDevice["name"];
    const char *name = jDevice["rate"];

    if (port > 2) {
        port = 2;
    }

    if (channel > 16) {
        channel = 16;
    }

    if (channel < 1) {
        channel = 1;
    }

    if ((rate < 10) || (rate > 1000)) {
        rate = 0;
    }

    return (Device(id, name, port - 1, channel, rate));
}

/** Parse array of Patches within a file
 *
 */
void Preset::parsePatches(File &file,
                          size_t startPosition,
                          size_t endPosition,
                          Device *device)
{
    file.seek(startPosition);

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
    const size_t capacity = 4096;
    const size_t capacityFilter = JSON_OBJECT_SIZE(20);
    StaticJsonDocument<capacity> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["post"] = true;
    filter["request"] = true;

    file.seek(startPosition);
    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Preset::parsePatch: deserializeJson() failed: %s",
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
    device->postMessages =
        parsePostMessages(jPatchItem["post"], device->getId());
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
    file.seek(startPosition);

    if (findElement(file, "\"responses\"", ARRAY, endPosition)) {
        do {
            size_t responseStartPosition = file.position();

            const size_t capacity = 1024;
            const size_t capacityFilter = 128;
            StaticJsonDocument<capacity> doc;
            StaticJsonDocument<capacityFilter> filter;

            filter["id"] = true;
            filter["length"] = true;
            filter["header"] = true;

            DeserializationError err = deserializeJson(
                doc, file, DeserializationOption::Filter(filter));

            if (err) {
                logMessage(
                    "Preset::parseResponses: deserializeJson() failed: %s",
                    err.c_str());
                return;
            }

            Response response;

            response.id = doc["id"].as<uint8_t>();
            response.length = doc["length"].as<uint16_t>();
            response.headers = parseResponseHeader(doc["header"]);

            //logMessage ("parseResponses: id=%d, length=%d", response.id, response.length);

            response.rules =
                parseRules(file, responseStartPosition, endPosition);
            device->responses.push_back(response);
        } while (file.findUntil(",", "]"));
    }
}

/** Parse response header JSON array
 *
 */
std::vector<uint8_t> Preset::parseResponseHeader(JsonArray jResponseHeader)
{
    std::vector<uint8_t> headerBytes;

    for (auto jByte : jResponseHeader) {
        if (jByte.is<char *>()) {
            uint8_t byte = strtol(jByte, 0, 16);
            headerBytes.push_back(byte);
        } else {
            uint8_t byte = jByte.as<uint8_t>();
            headerBytes.push_back(byte);
        }
    }

    //logData (headerBytes, "parseResponseHeader: parsed data:");

    return (headerBytes);
}

/** Parse request data JSON array
 *
 */
std::vector<uint8_t> Preset::parseRequest(JsonArray jRequest, uint8_t deviceId)
{
    std::vector<uint8_t> request =
        parseData(jRequest, deviceId, ElectraMessageType::sysex);

    return (request);
}

/** Parse Post patch messages
 *
 */
std::vector<Message> Preset::parsePostMessages(JsonArray jPostPatch,
                                               uint8_t deviceId)
{
    std::vector<Message> messages;

    for (auto jPostPatchMessage : jPostPatch) {
        Message message = parseMessage(jPostPatchMessage);
        message.setDeviceId(deviceId);
        messages.push_back(message);
    }

    return (messages);
}

/*--------------------------------------------------------------------------*/

/** Parse array of overlay items of given overlay
 *
 */
bool Preset::parseOverlayItems(File &file, Overlay *overlay)
{
    const size_t capacity = JSON_OBJECT_SIZE(3) + 256;
    StaticJsonDocument<capacity> doc;

    uint16_t value;
    const char *label;
    const char *bitmap;

    if (findElement(file, "\"items\"", ARRAY) == false) {
        logMessage("Preset::parseOverlayItems: items array not found");
        return (false);
    }

    do {
        DeserializationError err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            logMessage(
                "Preset::parseOverlayItems: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject item = doc.as<JsonObject>();

        if (item) {
            value = item["value"].as<uint16_t>();
            label = item["label"].as<char *>();
            bitmap = item["bitmap"].as<char *>();

            Bitmap bitmapLocation;

            if (bitmap) {
                bitmapLocation = parseBitmap(bitmap);
            } else {
                bitmapLocation.setX(NOT_SET_11);
            }

            if (!label) {
                label = "";
            }

            logMessage(
                "Preset::parseOverlayItems: value=%d, label=%s, bitmap.empty=%d",
                value,
                label,
                bitmapLocation.isEmpty());

            overlay->addItem(
                { value,
                  stringPool.saveItem(bitmapLocation.getAddress(), label) });
            //overlay->addItem ({ value, (uint32_t)0 });

        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

/*--------------------------------------------------------------------------*/

/** parse Group JSON object
 *
 */
Group Preset::parseGroup(JsonObject jGroup, uint8_t defaultGroupId)
{
    uint8_t groupId = jGroup["groupId"] | defaultGroupId;
    ;
    uint8_t pageId = jGroup["pageId"];
    Rectangle bounds = parseBounds(jGroup["bounds"]);
    ;
    const char *name = jGroup["name"];
    const char *rgb565Colour = jGroup["color"];

    Colour colour = ElectraColours::translateColour(rgb565Colour);

    if (pageId > 0) {
        pageId--;
    }

    return (Group(groupId, pageId, bounds, name, colour));
}

/*--------------------------------------------------------------------------*/

/** Parse individual Control object
 *
 */
Control Preset::parseControl(JsonObject jControl)
{
    uint16_t id = jControl["id"];
    uint8_t pageId = jControl["pageId"];
    const char *name = jControl["name"];
    const char *type = jControl["type"];
    const char *mode = jControl["mode"];
    const char *rgb565Colour = jControl["color"];
    const char *variant = jControl["variant"];
    uint8_t controlSetId = jControl["controlSetId"];
    bool visible = jControl["visible"] | true;

    ControlType controlType = Control::translateControlType(type);
    Colour colour = ElectraColours::translateColour(rgb565Colour);
    ControlMode controlMode = Control::translateControlMode(mode);
    Variant variantType = Control::translateVariant(variant);

    /* TODO: shift the ids down by one, this could be done better */
    if (pageId > 0) {
        pageId--;
    }

    if (controlSetId > 0) {
        controlSetId--;
    }

    return (Control(id,
                    pageId,
                    name,
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
                                       ControlType controlType)
{
    std::vector<Input> inputs;

    file.seek(startPosition);

    if (findElement(file, "\"inputs\"", ARRAY, endPosition)) {
        do {
            size_t valueStartPosition = file.position();

            inputs.push_back(parseInput(file, valueStartPosition, controlType));
        } while (file.findUntil(",", "]"));
    }

    return (inputs);
}

/** Parse an individual Control input
 *
 */
Input Preset::parseInput(File &file,
                         size_t startPosition,
                         ControlType controlType)
{
    Input input;
    const size_t capacity = 128;
    StaticJsonDocument<capacity> doc;

    file.seek(startPosition);

    DeserializationError err = deserializeJson(doc, file);

    if (err) {
        logMessage("Preset::parseInput: deserializeJson() failed: %s",
                   err.c_str());
        return (input);
    }
    size_t valueEndPosition = file.position();

    input = parseInput(controlType, doc.as<JsonObject>());
    file.seek(valueEndPosition);

    return (input);
}

/** Parse Input JSON object
 *
 */
Input Preset::parseInput(ControlType controlType, JsonObject jInput)
{
    uint8_t potId = jInput["potId"];
    const char *valueId = jInput["valueId"] | "";

    uint8_t valueIndex = translateValueId(controlType, valueId);

    if (potId > 0) {
        potId--;
    }

    if (potId >= MAX_POT_ID) {
        logMessage("parseInput: invalid pot: potId=%d", potId);
        potId = 0;
    }

    return (Input(valueIndex, potId));
}

/*--------------------------------------------------------------------------*/

/** Parse array of Value objects in the Control
 *
 */
std::vector<Value2> Preset::parseValues(File &file,
                                        size_t startPosition,
                                        size_t endPosition,
                                        Control *control)
{
    std::vector<Value2> values(Preset::getNumValues(control->getType()));

    file.seek(startPosition);

    if (findElement(file, "\"values\"", ARRAY, endPosition)) {
        do {
            size_t valueStartPosition = file.position();

            Value2 value = parseValue(file, valueStartPosition, control);
            values[value.getIndex()] = value;
        } while (file.findUntil(",", "]"));
    }

    return (values);
}

/* Parse individual Value out of the Value Array
 *
 */
Value2 Preset::parseValue(File &file, size_t startPosition, Control *control)
{
    Value2 value;

    file.seek(startPosition);

    const size_t capacity = 2048;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, file);

    if (err) {
        logMessage("Preset::parseValue: deserializeJson() failed: %s",
                   err.c_str());
        return (value);
    }

    size_t valueEndPosition = file.position();

    /* parse value JSON */
    value = parseValue(control, doc.as<JsonObject>());
    file.seek(valueEndPosition);

    return (value);
}

/** Parse Value JSON object
 *
 */
Value2 Preset::parseValue(Control *control, JsonObject jValue)
{
    int16_t defaultValue = 0;
    int16_t min = 0;
    int16_t max = 0;
    const char *defaultValueText = "";

    const char *valueId = jValue["id"] | "";
    uint8_t overlayId = jValue["overlayId"] | 0;
    const char *formatter = jValue["formatter"];
    const char *function = jValue["function"];

    ControlType controlType = control->getType();
    uint8_t valueIndex = translateValueId(controlType, valueId);

    bool minNotDefined = false;
    bool maxNotDefined = false;

    if (jValue["defaultValue"].is<int16_t>()) {
        defaultValue = jValue["defaultValue"].as<int16_t>();
    } else if (jValue["defaultValue"].is<char *>()) {
        defaultValueText = jValue["defaultValue"].as<char *>();
    }

    if (jValue["min"]) {
        min = jValue["min"].as<int16_t>();
    } else {
        minNotDefined = true;
    }

    if (jValue["max"]) {
        max = jValue["max"].as<int16_t>();
    } else {
        maxNotDefined = true;
    }

    Message message = parseMessage(jValue["message"]);

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
    if ((controlType != ControlType::pad)
        && (controlType != ControlType::list)) {
        if (defaultValue > max) {
            defaultValue = max;
        } else if (defaultValue < min) {
            defaultValue = min;
        }
    } else {
        if (defaultValueText) {
            if (strcmp(defaultValueText, "on") == 0) {
                defaultValue = message.getOnValue();
            } else if (strcmp(defaultValueText, "off") == 0) {
                defaultValue = message.getOffValue();
            }
        }
    }

    logMessage(
        "parseValue: id=%s, index=%d, defaultValue=%d, min=%d, max=%d, overlayId=%d, formatter=%s, function=%s",
        valueId,
        valueIndex,
        defaultValue,
        min,
        max,
        overlayId,
        formatter,
        function);

    /* TODO: make use overlay is null if nit defined and handle further down accordingly*/
    Overlay *overlay = getOverlay(overlayId);

    return (Value2(control,
                   valueId,
                   valueIndex,
                   defaultValue,
                   min,
                   max,
                   overlayId,
                   overlay,
                   message,
                   formatter,
                   function));
}

/*--------------------------------------------------------------------------*/

/** Parse message JSON object
 *
 */
Message Preset::parseMessage(JsonObject jMessage)
{
    uint8_t deviceId = jMessage["deviceId"] | 0;
    const char *type = jMessage["type"];
    int16_t parameterNumber = jMessage["parameterNumber"];
    int16_t min = jMessage["min"] | 0;
    int16_t max = jMessage["max"] | 0;
    int16_t value = jMessage["value"];
    int16_t offValue = jMessage["offValue"] | MIDI_VALUE_DO_NOT_SEND;
    int16_t onValue = jMessage["onValue"] | MIDI_VALUE_DO_NOT_SEND;
    bool lsbFirst = jMessage["lsbFirst"];
    const char *signModeInput = jMessage["signMode"];
    uint8_t bitWidth;

    ElectraMessageType electraMessageType = translateElectraMessageType(type);

    std::vector<uint8_t> data =
        parseData(jMessage["data"], parameterNumber, electraMessageType);

    SignMode signMode = translateSignMode(signModeInput);

    if (jMessage["bitWidth"]) {
        bitWidth = jMessage["bitWidth"].as<uint8_t>();
    } else {
        bitWidth = getDefaultbitWidth(electraMessageType);
    }

    if (min < 0) {
        min = 0;
    }

    if (max > 16383) {
        max = 16383;
    }

    if (electraMessageType == ElectraMessageType::program) {
        onValue = parameterNumber;
        offValue = MIDI_VALUE_DO_NOT_SEND;
        parameterNumber = 0;
    }

    logMessage(
        "parseMessage: device=%d, msgType=%s (%d), parameterId=%d, min=%d, max=%d, value=%d, offValue=%d, onValue=%d, lsbFirst=%d, signMode=%d, bitWidth=%d",
        deviceId,
        type,
        electraMessageType,
        parameterNumber,
        min,
        max,
        value,
        (offValue == MIDI_VALUE_DO_NOT_SEND) ? -1 : offValue,
        (onValue == MIDI_VALUE_DO_NOT_SEND) ? -1 : onValue,
        lsbFirst,
        signMode,
        bitWidth);

    return (Message(deviceId,
                    electraMessageType,
                    parameterNumber,
                    min,
                    max,
                    value,
                    offValue,
                    onValue,
                    data,
                    lsbFirst,
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

    file.seek(startPosition);

    if (findElement(file, "\"rules\"", ARRAY, endPosition)) {
        do {
            size_t valueStartPosition = file.position();

            rules.push_back(parseRule(file, valueStartPosition));

        } while (file.findUntil(",", "]"));
    }

    return (rules);
}

/** ParseRule within a file
 *
 */
Rule Preset::parseRule(File &file, size_t startPosition)
{
    Rule rule;

    file.seek(startPosition);

    const size_t capacity = 256;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, file);

    if (err) {
        logMessage("Preset::parseRule: deserializeJson() failed: %s",
                   err.c_str());
        return (rule);
    }

    size_t valueEndPosition = file.position();

    rule = parseRule(doc.as<JsonObject>());
    file.seek(valueEndPosition);

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
    if (jRule["parameterNumber"]) {
        parameterNumber = jRule["parameterNumber"].as<uint16_t>();
    } else {
        parameterNumber = jRule["id"].as<uint16_t>();
    }

    if (jRule["parameterBitPosition"]) {
        pPos = jRule["parameterBitPosition"].as<uint8_t>();
    } else {
        pPos = jRule["pPos"].as<uint8_t>();
    }

    if (jRule["byteBitPosition"]) {
        bPos = jRule["byteBitPosition"].as<uint8_t>();
    } else {
        bPos = jRule["bPos"].as<uint8_t>();
    }

    if (jRule["bitWidth"]) {
        size = jRule["bitWidth"].as<uint8_t>();
    } else if (jRule["size"]) {
        size = jRule["size"].as<uint8_t>();
    } else {
        size = 7;
    }

    type = jRule["type"].as<char *>();
    byte = jRule["byte"].as<uint16_t>();

    ElectraMessageType electraMessageType = translateElectraMessageType(type);

    if ((bPos + size) > 7) {
        logMessage(
            "parseRule: position of bits in the byte exceeds 7 bits: byteBitPosition=%d, bitWidth=%d",
            bPos,
            size);
    }

    if ((pPos + size) > 14) {
        logMessage(
            "parseRule: position of bits in the parameter exceeds 14 bits: parameterBitPosition=%d, bitWidth=%d",
            pPos,
            size);
    }
    //logMessage ("parseRule: rule: type=%d, parameterNumber=%d, byte=%d, parameterBitPosition=%d, byteBitPosition=%d, bitWidth=%d", electraMessageType, parameterNumber, byte, pPos, bPos, size);

    return (Rule(electraMessageType, parameterNumber, byte, pPos, bPos, size));
}

/** Parse JSON sysex data object
 *
 */
std::vector<uint8_t> Preset::parseData(JsonArray jData,
                                       int16_t parameterNumber,
                                       ElectraMessageType electraMessageType)
{
    std::vector<uint8_t> data;

    for (JsonVariant jByte : jData) {
        if (jByte.is<JsonObject>()) {
            const char *type = jByte["type"];
            //logMessage ("parseData: Placeholder type: %s", type);

            if (type) {
                if (strcmp(type, "value") == 0) {
                    data.push_back(VARIABLE_DATA);

                    JsonArray jRules = jByte["rules"];

                    if (!jRules || (jRules.size() == 0)) {
                        data.push_back((uint8_t)electraMessageType);
                        data.push_back(parameterNumber & 0x7F);
                        data.push_back(parameterNumber >> 7);
                        data.push_back(0);
                        data.push_back(0);
                        data.push_back(7);
                    } else {
                        for (JsonObject jRule : jRules) {
                            const char *type = jRule["type"].as<char *>();
                            uint16_t parameterId =
                                jRule["parameterNumber"].as<uint16_t>();
                            uint8_t pPos =
                                jRule["parameterBitPosition"].as<uint8_t>();
                            uint8_t bPos =
                                jRule["byteBitPosition"].as<uint8_t>();
                            uint8_t size = jRule["bitWidth"].as<uint8_t>();

                            ElectraMessageType electraMessageType =
                                translateElectraMessageType(type);

                            if (!jRule["bitWidth"]) {
                                size = 7;
                            }

                            data.push_back((uint8_t)electraMessageType);
                            data.push_back(parameterId & 0x7F);
                            data.push_back(parameterId >> 7);
                            data.push_back(pPos);
                            data.push_back(bPos);
                            data.push_back(size);
                        }
                    }

                    data.push_back(VARIABLE_DATA_END);
                } else if (strcmp(type, "checksum") == 0) {
                    data.push_back(CHECKSUM);
                    uint8_t start = jByte["start"].as<uint8_t>();
                    uint8_t length = jByte["length"].as<uint8_t>();
                    const char *algorithm = jByte["algorithm"].as<char *>();
                    ChecksumAlgorithm checksumAlgorithm =
                        translateAlgorithm(algorithm);

                    data.push_back((uint8_t)checksumAlgorithm);
                    data.push_back(start);
                    data.push_back(length);
                } else if (strcmp(type, "function") == 0) {
                    const char *functionName = jByte["name"].as<char *>();
                    luaFunctions.push_back(functionName);
                    data.push_back(LUAFUNCTION);
                    data.push_back((uint8_t)electraMessageType);
                    data.push_back(parameterNumber & 0x7F);
                    data.push_back(parameterNumber >> 7);
                    data.push_back(luaFunctions.size() - 1);
                }
            }
        } else {
            /*
			 * Add plain byte
			 */
            uint8_t byte;

            /*
			 * Support both Hex and Dec notation
			 */
            if (jByte.is<char *>()) {
                byte = strtol(jByte, 0, 16);
            } else {
                byte = jByte.as<uint8_t>();
            }

            /*
			 * Add byte, unless it is a SysEx start or stop byte
			 */
            if ((byte != 0xF0) && (byte != 0xF7)) {
                data.push_back(byte);
            }
        }
    }

    return (data);
}

/*--------------------------------------------------------------------------*/

/** Parse bounds JSON object
 *
 */
Rectangle Preset::parseBounds(JsonArray jBounds)
{
    return (Rectangle(jBounds[0].as<uint16_t>() + 13,
                      jBounds[1].as<uint16_t>() + 24,
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

    file.seek(startPosition);

    if (findElement(file, "\"bounds\"", ELEMENT, endPosition)) {
        const size_t capacity = JSON_ARRAY_SIZE(4) + 20;
        StaticJsonDocument<capacity> doc;

        DeserializationError err = deserializeJson(doc, file);
        bounds = parseBounds(doc.as<JsonArray>());
        //logMessage ("parseBounds: bounds=[%d %d %d %d]", bounds.x, bounds.y, bounds.width, bounds.height);

        if (err) {
            logMessage(
                "Preset::parseBounds: deserializeJson() of bounds failed: %s",
                err.c_str());
            return (bounds);
        }
    }
    return (bounds);
}

/*--------------------------------------------------------------------------*/

/** Parse bitmap data
 *
 */
Bitmap Preset::parseBitmap(const char *bitmap)
{
    uint8_t bytes[255] = {};

    base64_decode((char *)bytes, (char *)bitmap, strlen(bitmap));

    return (bitmapPool.saveBitmap(bytes));
}

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
uint8_t Preset::translateValueId(ControlType type, const char *valueId)
{
    if (!valueId) {
        return (0);
    }

    if (type == ControlType::adsr) {
        if (strcmp(valueId, "attack") == 0) {
            return (0);
        } else if (strcmp(valueId, "decay") == 0) {
            return (1);
        } else if (strcmp(valueId, "sustain") == 0) {
            return (2);
        } else if ((type == ControlType::adr)
                   && (strcmp(valueId, "release") == 0)) {
            return (2);
        } else if (strcmp(valueId, "release") == 0) {
            return (3);
        }
    }

    if (type == ControlType::adr) {
        if (strcmp(valueId, "attack") == 0) {
            return (0);
        } else if (strcmp(valueId, "decay") == 0) {
            return (1);
        } else if (strcmp(valueId, "release") == 0) {
            return (2);
        }
    }

    if (type == ControlType::dx7envelope) {
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

uint8_t Preset::getNumValues(ControlType type)
{
    switch (type) {
        case (ControlType::adr):
            return (3);

        case (ControlType::adsr):
            return (4);

        case (ControlType::dx7envelope):
            return (8);

        case (ControlType::fader):
        case (ControlType::list):
        case (ControlType::pad):
        default:
            return (1);
    }
}

uint8_t Preset::getDefaultbitWidth(ElectraMessageType electraMessageType)
{
    if (electraMessageType == ElectraMessageType::cc14) {
        return (14);
    } else if (electraMessageType == ElectraMessageType::nrpn) {
        return (14);
    }

    return (7);
}

/*--------------------------------------------------------------------------*/

bool findElement(File &file,
                 const char *elementName,
                 ElementType elementType,
                 size_t endPosition)
{
    if (file.find(elementName) == false) {
        logMessage("findElement: no %s defined", elementName);
        return (false);
    }
    if (file.find(":") == false) {
        logMessage("findElement: %s is not an element", elementName);
        return (false);
    }
    if (elementType == ARRAY) {
        if (file.find("[") == false) {
            logMessage("findElement: %s is not an array", elementName);
            return (false);
        }
    } else if (elementType == OBJECT) {
        if (file.find("{") == false) {
            logMessage("findElement: %s is not an object", elementName);
            return (false);
        }
    }

    if ((endPosition != 0) && (file.position() > endPosition)) {
        logMessage(
            "findElement: not present in the range: element=%s, endPosition=%d",
            elementName,
            endPosition);
        return (false);
    }

    return (true);
}

bool isElementEmpty(File &file)
{
    size_t pos = file.position();
    uint8_t c;
    bool rc = false;

    while ((c = file.read()) && (c <= 32)) {
        logChars(&c, 1);
    }

    if ((c == ']') || (c == '}')) {
        rc = true;
    }

    file.seek(pos);
    return (rc);
}
