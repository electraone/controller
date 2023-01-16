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
                       Message::Type type,
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

/*
 * Set the associated application sandbox
 */
void ParameterMap::setAppSandbox(const char *newAppSandbox)
{
    copyString(appSandbox, newAppSandbox, 20);
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
                                       Message::Type type,
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
                               Message::Type type,
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
                                Message::Type type,
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
                                    Message::Type type,
                                    uint16_t parameterNumber,
                                    uint16_t midiValue,
                                    Origin origin)
{
    System::logger.write(
        TRACE,
        "ParameterMap::setValue: deviceId=%d, type=%d, parameterNumber=%d, midiValue=%d, origin=%d",
        deviceId,
        type,
        parameterNumber,
        midiValue,
        origin);

    // These messages are not device specific
    if (type == Message::Type::start || type == Message::Type::stop
        || type == Message::Type::tune) {
        deviceId = 0xff;
    }

    uint32_t hash = calculateHash(deviceId, type, parameterNumber);
    LookupEntry *entry = findAndCache(hash);

    if (entry) {
        if ((type != Message::Type::relcc) && (entry->midiValue == midiValue)) {
            return (nullptr);
        }

        entry->dirty = true;
        entry->midiValue = midiValue;

        if (midiValue != MIDI_VALUE_DO_NOT_SEND) {
            if (onChange) {
                onChange(entry, origin);
            }
        }
#ifdef DEBUG
        parameterMap.print();
#endif
        return (entry);
    }

    return (nullptr);
}

/** Applies (ORs) the value of an existing entry
 *  If not found, it does nothing.
 *  It sets the dirty flag on
 */
LookupEntry *ParameterMap::applyToValue(uint8_t deviceId,
                                        Message::Type type,
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
        entry.messageDestination = std::vector<ControlValue *>();
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
    System::logger.write(TRACE, "--<Parameter Map:start>--");
    for (auto &entry : entries) {
        System::logger.write(
            TRACE,
            "ParameterMap entry: deviceId: %d, type=%d, parameterNumber=%d,"
            " midiValue=%d, numDestinations=%d",
            getDeviceId(entry.hash),
            getType(entry.hash),
            getParameterNumber(entry.hash),
            entry.midiValue,
            entry.messageDestination.size());
        for (auto &md : entry.messageDestination) {
            System::logger.write(TRACE,
                                 "control: %s, value handle: %s",
                                 md->getControl()->getName(),
                                 md->getId());
        }
    }
    System::logger.write(TRACE, "--<Parameter Map:end>--");
}

/*
 * Saving and loading the parameterMap functions
 */
void ParameterMap::keep(void)
{
    char mapStateFilename[MAX_FILENAME_LENGTH + 1];
    prepareMapStateFilename(mapStateFilename, MAX_FILENAME_LENGTH);
    System::logger.write(
        INFO, "ParameterMap::keep: filename=%s", mapStateFilename);
    save(mapStateFilename);
}

void ParameterMap::save(const char *filename)
{
    serialize(filename);
}

void ParameterMap::serialize(const char *filename)
{
    File file = Hardware::sdcard.createOutputStream(
        filename, FILE_WRITE | O_CREAT | O_TRUNC);

    if (!file) {
        System::logger.write(
            ERROR,
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

        System::logger.write(
            TRACE,
            "ParameterMap::serializeMap: entry: deviceI=%d, type=%d, parameterNumber=%d, midiValue=%d",
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

bool ParameterMap::recall(void)
{
    bool status = false;
    char mapStateFilename[MAX_FILENAME_LENGTH + 1];
    prepareMapStateFilename(mapStateFilename, MAX_FILENAME_LENGTH);
    System::logger.write(
        INFO, "ParameterMap::recall: filename=%s", mapStateFilename);
    if (Hardware::sdcard.exists(mapStateFilename)) {
        status = load(mapStateFilename);
    }
    return (status);
}

void ParameterMap::forget(void)
{
    char mapStateFilename[MAX_FILENAME_LENGTH + 1];
    prepareMapStateFilename(mapStateFilename, MAX_FILENAME_LENGTH);
    if (Hardware::sdcard.deleteFile(mapStateFilename)) {
        System::logger.write(ERROR,
                             "ParameterMap::forget: cannot remove file: %s",
                             mapStateFilename);
    }
}

bool ParameterMap::load(const char *filename)
{
    System::logger.write(
        INFO, "ParameterMap::load: file: filename=%s", filename);

    File file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        System::logger.write(
            ERROR, "ParameterMap::load: cannot open setup file: %s", filename);
        return (false);
    }

    file.setTimeout(100);

    if (!parse(file)) {
        System::logger.write(
            ERROR,
            "ParameterMap::load: cannot parse setup: filename=%s",
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
        System::logger.write(
            ERROR, "ParameterMap::parseParameters: parameters array not found");
        return (false);
    }

    do {
        DeserializationError err = deserializeJson(doc, file);

        if (err && (err.code() != DeserializationError::InvalidInput)) {
            System::logger.write(
                TRACE,
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
                     (Message::Type)messageType,
                     parameterNumber,
                     midiValue,
                     Origin::file);

            System::logger.write(
                TRACE,
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
    System::logger.write(TRACE,
                         "ParameterMap::addWindow: window=%s, address=%x",
                         windowToAdd->getName(),
                         windowToAdd);
    windows.push_back(windowToAdd);
    return (true);
}

void ParameterMap::removeWindow(ParameterMapWindow *windowToRemove)
{
    System::logger.write(TRACE,
                         "ParameterMap::removeWindow: window=%s, address=%x",
                         windowToRemove->getName(),
                         windowToRemove);

    for (auto i = windows.begin(); i != windows.end(); i++) {
        if (*i == windowToRemove) {
            windows.erase(i);
            System::logger.write(TRACE,
                                 "ParameterMap::removeWindow: window removed");
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
        System::logger.write(
            TRACE,
            "ParameterMap::listWindows: index=%d, name=%s, address=%x",
            index,
            window->getName(),
            window);
        index++;
    }
}

void ParameterMap::enable(void)
{
    InstanceCallback<void(void)>::callbackFunction =
        std::bind(&ParameterMap::repaintParameterMap, this);
    TaskCallback repaintTaskCallback =
        static_cast<TaskCallback>(InstanceCallback<void(void)>::callback);

    System::tasks.addTask(repaintParameterMapTask);
    repaintParameterMapTask.set(40000, TASK_FOREVER, repaintTaskCallback);
    repaintParameterMapTask.enable();
}

void ParameterMap::disable(void)
{
    System::tasks.disableRepaintGraphics();
    System::tasks.deleteTask(repaintParameterMapTask);
    System::tasks.clearRepaintGraphics();
    System::tasks.enableRepaintGraphics();
}

/*
 * Scheduller task to repaint dirty parameters
 */
void ParameterMap::repaintParameterMap(void)
{
    for (auto &mapEntry : entries) {
        if (mapEntry.dirty) {
            System::logger.write(
                TRACE,
                "repaintParameterMap: dirty entry found: device=%d, type=%d, "
                "parameterNumber=%d, midiValue=%d",
                getDeviceId(mapEntry.hash),
                getType(mapEntry.hash),
                getParameterNumber(mapEntry.hash),
                mapEntry.midiValue,
                mapEntry.dirty);

            if (mapEntry.midiValue != MIDI_VALUE_DO_NOT_SEND) {
                for (auto &messageDestination : mapEntry.messageDestination) {
                    for (const auto &window : windows) {
                        Component *rc = window->getOwnedContent();
                        Component *c = rc->findChildById(
                            messageDestination->getControl()->getId());

                        if (c) {
                            System::logger.write(
                                TRACE,
                                "repaintParameterMap: repainting component: "
                                "component: %s, controlId=%d, valueId=%s",
                                c->getName(),
                                messageDestination->getControl()->getId(),
                                messageDestination->getId());
                            ControlComponent *cc =
                                dynamic_cast<ControlComponent *>(c);

                            if (cc) {
                                cc->onMidiValueChange(
                                    *messageDestination,
                                    mapEntry.midiValue,
                                    messageDestination->getHandle());
                            }
                        }
                    }
                }
            }
            mapEntry.dirty = false;
        }
    }
}

void ParameterMap::prepareMapStateFilename(char *buffer, size_t maxLength)
{
    snprintf(buffer, maxLength, "%s/%s.map", appSandbox, projectId);
}
