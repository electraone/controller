#pragma once

#include "Component.h"
#include "Model/Presets.h"
#include "Setup/Setup.h"
#include "UiDelegate.h"
#include "BankButton.h"
#include "PresetButton.h"

class PresetSelection : public Component
{
public:
    PresetSelection(const Presets &newPresets,
                    const PresetBanks &newPresetBanks,
                    UiDelegate &newDelegate);
    virtual ~PresetSelection() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void setActiveLabel(uint8_t newActivePage);
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIcon(Graphics &g, uint16_t x, uint16_t y);

    void setActiveBank(uint8_t newActiveBank);

    const Presets &presets;
    const PresetBanks &presetBanks;
    UiDelegate &delegate;
    uint8_t active;
    uint8_t activeBank;

    BankButton *bankButton[6];
    PresetButton *button[12];

    static constexpr uint8_t topPadding = 50;
};
