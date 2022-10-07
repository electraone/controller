#pragma once

#include "helpers.h"
#include "Colours.h"

struct PresetBank {
    PresetBank() : id(0), colour(Colours::white)
    {
        copyString(name, "", MaxNameLength);
    }

    PresetBank(uint8_t newId, const char *newName, uint16_t newColour)
        : id(newId), colour(newColour)
    {
        copyString(name, newName, MaxNameLength);
    }

    static const int MaxNameLength = 20;

    uint8_t id;
    char name[MaxNameLength + 1];
    uint16_t colour;
};
