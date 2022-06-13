#pragma once

#include "Preset.h"
#include "Hardware.h"
#include "LocalFile.h"

class Presets
{
public:
    explicit Presets(const char *newAppSandbox);
    virtual ~Presets() = default;

    void assignPresetNames(uint8_t bankNumber);
    void sendList(void);

    bool loadPresetById(int presetId);
    bool loadPreset(LocalFile file);
    void reset(void);
    void runPresetLuaScript(void);

    void setBankNumberAndSlot(uint8_t presetId);
    void setBankNumberAndSlot(uint8_t newBankNumber, uint8_t newSlot);
    uint8_t getPresetId(void) const;
    void setCurrentSlot(uint8_t newSlot);
    uint8_t getCurrentSlot(void) const;
    void setCurrentBankNumber(uint8_t newBankNumber);
    uint8_t getCurrentBankNumber(void) const;

    Preset preset;

private:
    static constexpr uint8_t NumPresetsInBank = 12;

    void setDefaultFiles(uint8_t newBankNumber, uint8_t newSlot);

    const char *appSandbox;

    uint8_t currentBankNumber;
    uint8_t currentSlot;
    bool readyForPresetSwitch;

    char tempFilename[MAX_FILENAME_LENGTH + 1];
    char presetNames[NumPresetsInBank][Preset::MaxNameLength + 1];
};
