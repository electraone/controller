#pragma once

#include "Enums.h"

struct UiFeatures {
    UiFeatures()
        : touchSwitchControlSets(true),
          resetActiveControlSet(false),
          activeControlSetType(ActiveControlSetType::dim)
    {
    }

    bool touchSwitchControlSets;
    bool resetActiveControlSet;
    ActiveControlSetType activeControlSetType;
};
