#include "ParameterMap.h"
#include "ParameterMapWindow.h"
#include "ControlComponent.h"
#include "JsonTools.h"

ParameterMap parameterMap;

/** Message hash function.
 *  creates a hash value for given message attributes. This is used for lookups
 *  in ParameterMap unordered_map.
 */
uint32_t calculateHash(uint8_t deviceId,
                       ElectraMessageType type,
                       uint16_t parameterNumber)
{
    return (parameterNumber + (((uint8_t)type) << 16) + (deviceId << 24));
}

uint8_t getDeviceId(uint32_t hash)
{
    return (hash >> 24);
}

uint8_t getType(uint32_t hash)
{
    return ((hash >> 16) & 0xFF);
}

uint16_t getParameterNumber(uint32_t hash)
{
    return (hash & 0xFFFF);
}

/** ParameterMap
 *
 */
ParameterMap::ParameterMap() : lastRead(0), lastWritten(0)
{
}

/*
 * Set the associated project Id
 */
void ParameterMap::setProjectId(const char *newProjectId)
{
    copyString(projectId, newProjectId, 20);
}

/** locate and cache entry in the ParameterMap
 *
 */
LookupEntry *ParameterMap::findAndCache(uint32_t hash)
{
    uint16_t i = 0;

    if (entries[lastRead].hash == hash) {
        return (&entries[lastRead]);
    }

    for (auto &entry : entries) {
        if (entry.hash == hash) {
            lastRead = i;
            return (&entry);
        }
        i++;
    }

    return (nullptr);
}

/** Always returns a pointer to an entry
 *  If it does not exist, it will created and returned
 */
LookupEntry *ParameterMap::getOrCreate(uint8_t deviceId,
                                       ElectraMessageType type,
                                       uint16_t parameterNumber)
{
    uint32_t hash = calculateHash(deviceId, type, parameterNumber);
    LookupEntry *entry = findAndCache(hash);

    // return found entry
    if (entry) {
        return (entry);
    }

    // or return a newly created entry
    // TODO: MessageDestination pointers might not be initialized
    entries.push_back(LookupEntry(hash, 0));
    return (&entries.back());
}

/** Return entry of the ParameterMap
 *
 */
LookupEntry *ParameterMap::get(uint8_t deviceId,
                               ElectraMessageType type,
                               uint16_t parameterNumber)
{
    uint32_t hash = calculateHash(deviceId, type, parameterNumber);

    return (findAndCache(hash));
}

/** Returns a value of an existing entry
 *  if not found, 0 is returned.
 *
 */
uint16_t ParameterMap::getValue(uint8_t deviceId,
                                ElectraMessageType type,
                                uint16_t parameterNumber)
{
    uint32_t hash = calculateHash(deviceId, type, parameterNumber);
    LookupEntry *entry = findAndCache(hash);

    if (entry) {
        return (entry->midiValue);
    }

    return (0);
}

/** Sets the value of an existing entry
 *  If not found, it does nothing.
 *  It sets the dirty flag on
 */
LookupEntry *ParameterMap::setValue(uint8_t deviceId,
                                    ElectraMessageType type,
                                    uint16_t parameterNumber,
                                    uint16_t midiValue,
                                    Origin origin)
{
#ifdef DEBUG
    logMessage(
        "ParameterMap::setValue: deviceId=%d, type=%d, parameterNumber=%d, midiValue=%d, origin=%d",
        deviceId,
        type,
        parameterNumber,
        midiValue,
        origin);
#endif
    uint32_t hash = calculateHash(deviceId, type, parameterNumber);
    LookupEntry *entry = findAndCache(hash);

    if (entry) {
        if (entry->midiValue != midiValue) {
            entry->dirty = true;
            entry->midiValue = midiValue;

            if (onChange) {
                onChange(entry, origin);
            }

#ifdef DEBUG
            parameterMap.print();
#endif

            return (entry);
        }
    }

    return (nullptr);
}

/** Applies (ORs) the value of an existing entry
 *  If not found, it does nothing.
 *  It sets the dirty flag on
 */
LookupEntry *ParameterMap::applyToValue(uint8_t deviceId,
                                        ElectraMessageType type,
                                        uint16_t parameterNumber,
                                        uint16_t midiValueFragment,
                                        Origin origin)
{
    uint32_t hash = calculateHash(deviceId, type, parameterNumber);
    LookupEntry *entry = findAndCache(hash);

    if (entry) {
        entry->dirty = true;
        entry->midiValue |= midiValueFragment;

        if (onChange) {
            onChange(entry, origin);
        }

        return (entry);
    }

    return (nullptr);
}

/** Resets all device values to 0
 *  The dirty flag is set for all entries
 */
void ParameterMap::resetDeviceValues(uint8_t deviceId)
{
    for (auto &entry : entries) {
        if ((entry.hash >> 24) == deviceId) {
            entry.midiValue = 0;
            entry.dirty = true;
        }
    }
}

/** Resets and re-allocates the parameterMap
 *
 */
void ParameterMap::reset(void)
{
    setProjectId("undefined");

    for (auto &entry : entries) {
        entry.messageDestination = std::vector<MessageDestination>();
    }
    parameterMap.entries = std::vector<LookupEntry>();

    lastRead = 0;
    lastWritten = 0;
}

/** Prints content of the ParameterMap
 *
 */
void ParameterMap::print(void)
{
    logMessage("--<Parameter Map:start>--");
    for (auto &entry : entries) {
        logMessage(
            "ParameterMap entry: deviceId: %d, type=%d, parameterNumber=%d,"
            " midiValue=%d, numDestinations=%d",
            getDeviceId(entry.hash),
            getType(entry.hash),
            getParameterNumber(entry.hash),
            entry.midiValue,
            entry.messageDestination.size());
    }
    logMessage("--<Parameter Map:end>--");
}

/*
 * Saving and loading the parameterMap functions
 */
void ParameterMap::save(const char *filename)
{
    serialize(filename);
}

void ParameterMap::serialize(const char *filename)
{
    File file = Hardware::sdcard.createOutputStream(
        filename, FILE_WRITE | O_CREAT | O_TRUNC);

    if (!file) {
        logMessage(
            "ParameterMap::serialize: cannot open the patch file for writing");
        return;
    }

    file.print("{");
    serializeRoot(file);
    serializeMap(file);
    file.print("}");

    file.close();
}

void ParameterMap::serializeMap(File &file)
{
    bool firstRecord = true;

    file.print(",\"parameters\":[");
    for (auto entry : parameterMap.entries) {
        if (!firstRecord) {
            file.print(",");
        } else {
            firstRecord = false;
        }

        file.print("{\"deviceId\":");
        file.print(getDeviceId(entry.hash));
        file.print(",\"messageType\":");
        file.print(getType(entry.hash));
        file.print(",\"parameterNumber\":");
        file.print(getParameterNumber(entry.hash));
        file.print(",\"midiValue\":");
        file.print(entry.midiValue);
        file.print("}");

        logMessage(
            "entry: deviceI=%d, type=%d, parameterNumber=%d, midiValue=%d",
            getDeviceId(entry.hash),
            getType(entry.hash),
            getParameterNumber(entry.hash),
            entry.midiValue);
    }
    file.print("]");
}

void ParameterMap::serializeRoot(File &file)
{
    file.print("\"version\":1,");
    file.print("\"projectId\":\"");
    file.print(projectId);
    file.print("\"");
}

bool ParameterMap::load(const char *filename)
{
    logMessage("ParameterMap::load: file: filename=%s", filename);

    File file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        logMessage("ParameterMap::load: cannot open setup file: %s", filename);
        return (false);
    }

    file.setTimeout(100);

    if (!parse(file)) {
        logMessage("ParameterMap::load: cannot parse setup: filename=%s",
                   filename);
        file.close();
        return (false);
    } else {
        file.close();
    }

    return (true);
}

bool ParameterMap::parse(File &file)
{
    parseParameters(file);
    return (true);
}

bool ParameterMap::parseParameters(File &file)
{
    const size_t capacity = JSON_OBJECT_SIZE(3) + 512;
    StaticJsonDocument<capacity> doc;

    uint8_t deviceId;
    uint8_t messageType;
    uint16_t parameterNumber;
    uint16_t midiValue;

    if (findElement(file, "\"parameters\"", ARRAY) == false) {
        logMessage("ParameterMap::parseParameters: parameters array not found");
        return (false);
    }

    do {
        DeserializationError err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            logMessage(
                "ParameterMap::parseParameters: deserializeJson() failed: %s",
                err.c_str());
            return (false);
        }

        JsonObject item = doc.as<JsonObject>();

        if (item) {
            deviceId = item["deviceId"].as<uint8_t>();
            messageType = item["messageType"].as<uint8_t>();
            parameterNumber = item["parameterNumber"].as<uint16_t>();
            midiValue = item["midiValue"].as<uint16_t>();

            setValue(deviceId,
                     (ElectraMessageType)messageType,
                     parameterNumber,
                     midiValue,
                     Origin::internal);

            logMessage(
                "ParameterMap::parseParameters: deviceId=%d, messageType=%d, "
                "parameterNumber=%d, midiValue=%d",
                deviceId,
                messageType,
                parameterNumber,
                midiValue);
        } else {
            break;
        }
    } while (file.findUntil(",", "]"));

    return (true);
}

bool ParameterMap::addWindow(ParameterMapWindow *windowToAdd)
{
    logMessage("Pmap: addWindow: window=%s, address=%x",
               windowToAdd->getName(),
               windowToAdd);
    windows.push_back(windowToAdd);
    return (true);
}

void ParameterMap::removeWindow(ParameterMapWindow *windowToRemove)
{
    logMessage("Pmap: removeWindow: window=%s, address=%x",
               windowToRemove->getName(),
               windowToRemove);

    for (auto i = windows.begin(); i != windows.end(); i++) {
        if (*i == windowToRemove) {
            windows.erase(i);
            logMessage("Pmap: window removed");
            // note: iterator is invalid!
            return;
        }
    }
}

/**
 * List all registered windows
 */
void ParameterMap::listWindows(void)
{
    uint8_t index = 0;

    for (const auto &window : windows) {
        logMessage("Pmap: listWindows: index=%d, name=%s, address=%x",
                   index,
                   window->getName(),
                   window);
        index++;
    }
}

void ParameterMap::enable(void)
{
    TaskClassCallback<void(void)>::callbackFunction =
        std::bind(&ParameterMap::repaintParameterMap, this);
    TaskCallback repaintTaskCallback =
        static_cast<TaskCallback>(TaskClassCallback<void(void)>::callback);

    System::tasks.addTask(repaintParameterMapTask);
    repaintParameterMapTask.set(40000, TASK_FOREVER, repaintTaskCallback);
    repaintParameterMapTask.enable();
}

void ParameterMap::disable(void)
{
    System::tasks.disableRepaintGraphics();
    System::tasks.deleteTask(repaintParameterMapTask);
    System::tasks.flushRepaintGraphics();
    System::tasks.enableRepaintGraphics();
}

/*
 * Scheduller task to repaint dirty parameters
 */
void ParameterMap::repaintParameterMap(void)
{
    for (auto &mapEntry : entries) {
        if (mapEntry.dirty) {
#ifdef DEBUG
            logMessage(
                "repaintParameterMap: dirty entry found: device=%d, type=%d, "
                "parameterNumber=%d, midiValue=%d",
                getDeviceId(mapEntry.hash),
                getType(mapEntry.hash),
                getParameterNumber(mapEntry.hash),
                mapEntry.midiValue,
                mapEntry.dirty);
#endif

            for (auto &messageDestination : mapEntry.messageDestination) {
                for (const auto &window : windows) {
                    Component *rc = window->getOwnedContent();
                    Component *c =
                        rc->findChildById(messageDestination.control->getId());

                    if (c) {
#ifdef DEBUG
                        logMessage(
                            "repaintParameterMap: repainting component: component: %s, controlId=%d, value=%s",
                            c->getName(),
                            messageDestination.control->getId(),
                            messageDestination.value->getId());
#endif
                        ControlComponent *cc =
                            dynamic_cast<ControlComponent *>(c);

                        if (cc) {
                            cc->onMidiValueChange(
                                *messageDestination.value,
                                mapEntry.midiValue,
                                messageDestination.value->getHandle());
                        }
                    }
                }
            }
            mapEntry.dirty = false;
        }
    }
}
