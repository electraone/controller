#pragma once

#include "Preset.h"
#include "Hardware.h"
#include "LocalFile.h"

class Presets
{
public:
    explicit Presets(const char *newAppSandbox);
    virtual ~Presets() = default;

    void assignPresetNames(uint8_t presetBankId);
    void sendList(void);

    bool loadPresetById(int presetId);
    bool loadPreset(LocalFile file);
    void reset(void);
    void runPresetLuaScript(void);

    Preset preset;

private:
    static constexpr uint8_t NumPresetsInBank = 12;

    const char *appSandbox;

    uint8_t currentPresetBank;
    uint8_t currentPreset;
    bool readyForPresetSwitch;

    char tempFilename[MAX_FILENAME_LENGTH + 1];
    char presetNames[NumPresetsInBank][Preset::MaxNameLength + 1];
};
