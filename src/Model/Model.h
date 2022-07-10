#pragma once

#include "Presets.h"
#include "Snapshots.h"
#include "Info.h"

struct Model {
    explicit Model(const char *newAppSandbox)
        : presets(newAppSandbox),
          snapshots(newAppSandbox),
          currentPreset(presets.preset)
    {
    }

    virtual ~Model() = default;

    Presets presets;
    Snapshots snapshots;
    Preset &currentPreset;
    Info info;
};
