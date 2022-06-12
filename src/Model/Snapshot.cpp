#include "Snapshot.h"

Snapshot::Snapshot(const char *filename) : colour(3)
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

uint8_t Snapshot::getColour(void) const
{
    return colour;
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

    colour = ElectraColours::translateColour(doc["color"].as<char *>());
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
