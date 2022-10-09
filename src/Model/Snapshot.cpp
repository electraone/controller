#include "Snapshot.h"

Snapshot::Snapshot(const char *filename) : colour(Colours565::white)
{
    logMessage("Snapshots: file: filename=%s", filename);

    File file = Hardware::sdcard.createInputStream(filename);

    if (!file) {
        logMessage("Snapshots: cannot open snapshot file: %s", filename);
        return;
    }

    file.setTimeout(100);

    parse(file);

    file.close();
}

const char *Snapshot::getName(void) const
{
    return name;
}

uint32_t Snapshot::getColour(void) const
{
    return colour;
}

uint16_t Snapshot::getColour565(void) const
{
    return (Colours888::toRGB565(colour));
}

bool Snapshot::parse(File &file)
{
    const size_t capacityRootObjects = JSON_OBJECT_SIZE(1) + 256;
    const size_t capacityFilter = JSON_OBJECT_SIZE(1) + 100;
    StaticJsonDocument<capacityRootObjects> doc;
    StaticJsonDocument<capacityFilter> filter;

    filter["name"] = true;
    filter["color"] = true;

    if (file.seek(0) == false) {
        logMessage("Snapshots::parse: cannot rewind the file");
        return (false);
    }

    DeserializationError err =
        deserializeJson(doc, file, DeserializationOption::Filter(filter));

    if (err) {
        logMessage("Snapshots::parse: parsing failed: %s", err.c_str());
        return (false);
    }

    colour = Colours565::fromString(doc["color"].as<char *>());
    copyString(name, doc["name"].as<char *>(), MAX_SNAPSHOT_NAME_LENGTH);

    return true;
}

/*
void sendSnapshotMessages(const char *projectId,
                          uint8_t bankNumber,
                          uint8_t slot)
{
    char filename[MAX_FILENAME_LENGTH + 1];
    composeSnapshotFilename(filename, projectId, bankNumber, slot);

    logMessage("file to load: %s", filename);

    if (Hardware::sdcard.exists(filename)) {
        parameterMap.load(filename);
        electraApp.sendAllControls();
    } else {
        logMessage("an empty slot: %s", filename);
    }
}
*/
