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
    uint16_t getColour(void) const;

private:
    bool parse(File &file);

    char name[MAX_SNAPSHOT_NAME_LENGTH + 1];
    uint16_t colour;
};
