#pragma once

#include "helpers.h"
#include "Colours.h"

struct PresetBank {
    PresetBank() : id(0), colour(ElectraColours::white)
    {
        copyString(name, "", MaxNameLength);
    }

    PresetBank(uint8_t newId, const char *newName, Colour newColour)
        : id(newId), colour(newColour)
    {
        copyString(name, newName, MaxNameLength);
    }

    static const int MaxNameLength = 20;

    uint8_t id;
    char name[MaxNameLength + 1];
    Colour colour;
};
