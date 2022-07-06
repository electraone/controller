#pragma once

#include "Window.h"
#include "PresetSelection.h"
#include "Model/Presets.h"
#include "UiApi.h"

class PresetSelectionWindow : public Window
{
private:
    PresetSelectionWindow(const Presets &presets,
                          const PresetBanks &presetBanks,
                          UiApi &newUiApi)
        : uiApi(newUiApi), content(nullptr)
    {
        content = new PresetSelection(presets, presetBanks, uiApi);

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
            uiApi.presetSelection_openUsbHostPorts();
        } else if (buttonId == 4) {
            uiApi.presetSelection_close();
        } else if (buttonId == 5) {
            uiApi.presetSelection_openPageSelection();
        }
    }

    // Factory function
    static PresetSelectionWindow *
        createPresetSelectionWindow(const Presets &presets,
                                    const PresetBanks &presetBanks,
                                    UiApi &newUiApi)
    {
        return new PresetSelectionWindow(presets, presetBanks, newUiApi);
    }

private:
    UiApi &uiApi;
    PresetSelection *content;
};
