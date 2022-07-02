#pragma once

#include "Window.h"
#include "PresetSelection.h"
#include "Model/Presets.h"
#include "UiDelegate.h"

class PresetSelectionWindow : public Window
{
private:
    PresetSelectionWindow(const Presets &presets,
                          const PresetBanks &presetBanks,
                          UiDelegate &newDelegate)
        : delegate(newDelegate), content(nullptr)
    {
        content = new PresetSelection(presets, presetBanks, newDelegate);

        if (content) {
            setName("PresetSelectionWindow");
            setOwnedContent(content);
        }
        setVisible(true);
    }

public:
    virtual ~PresetSelectionWindow() = default;

    // API

    // Events
    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            delegate.closePresetSelection();
            delegate.openUsbHostPorts();
        } else if (buttonId == 4) {
            delegate.closePresetSelection();
        } else if (buttonId == 5) {
            delegate.closePresetSelection();
            delegate.openPageSelection();
        }
    }

    // Factory function
    static PresetSelectionWindow *
        createPresetSelectionWindow(const Presets &presets,
                                    const PresetBanks &presetBanks,
                                    UiDelegate &newDelegate)
    {
        return new PresetSelectionWindow(presets, presetBanks, newDelegate);
    }

private:
    UiDelegate &delegate;
    PresetSelection *content;
};
