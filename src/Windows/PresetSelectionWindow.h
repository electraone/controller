#pragma once

#include "Window.h"
#include "PresetSelection.h"
#include "Model/Presets.h"
#include "UiDelegate.h"
#include "Hardware.h"

class PresetSelectionWindow : public Window
{
private:
    PresetSelectionWindow(const Presets &presets,
                          const PresetBanks &presetBanks,
                          UiDelegate &newDelegate)
        : delegate(newDelegate), ps(nullptr)
    {
        PresetSelection *ps =
            new PresetSelection(presets, presetBanks, newDelegate);

        if (ps) {
            setOwnedContent(ps);
            setVisible(true);
        }

        setName("presetSelectionWindow");
    }

public:
    virtual ~PresetSelectionWindow() = default;

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            delegate.closePresetSelection();
            delegate.openUsbHostPorts();
        } else if (buttonId == 4) {
            delegate.closePresetSelection();
        } else if (buttonId == 5) {
            delegate.closeUsbHostPorts();
            delegate.openPageSelection();
        }
    }

    static PresetSelectionWindow *
        createPresetSelectionWindow(const Presets &presets,
                                    const PresetBanks &presetBanks,
                                    UiDelegate &newDelegate)
    {
        PresetSelectionWindow *psw =
            new PresetSelectionWindow(presets, presetBanks, newDelegate);

        if (psw) {
            psw->setName("PresetSelection");
            psw->repaint();
        }

        return (psw);
    }

private:
    UiDelegate &delegate;
    PresetSelection *ps;
};
