#pragma once

#include "Hardware.h"
#include <ArduinoJson.h>

#define MAX_SNAPSHOT_NAME_LENGTH 14

class Snapshot
{
public:
    Snapshot(const char *filename);
    virtual ~Snapshot() = default;

    const char *getName(void) const;
    uint8_t getColour(void) const;

private:
    bool parse(File &file);

    char name[MAX_SNAPSHOT_NAME_LENGTH + 1];
    uint8_t colour;
};

/*
void sendSnapshotMessages(const char *projectId,
                          uint8_t bankNumber,
                          uint8_t slot);
*/
