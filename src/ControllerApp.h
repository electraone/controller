#pragma once

#include "App.h"
#include "MainWindow.h"
#include "MidiInputCallback.h"
#include "MidiCallbacks.h"
#include "PageView.h"
#include "Preset.h"
#include "Ui.h"
#include "UiDelegate.h"
#include "Midi.h"

Preset *luaPreset = nullptr;
UiDelegate *luaDelegate = nullptr;

class Controller : public App, private MidiInputCallback
{
public:
    Controller()
        : mainWindow(MainWindow(preset)),
          delegate(nullptr),
          currentPreset(0),
          currentPresetBank(0),
          readyForPresetSwitch(true),
          midi(preset)
    {
    }

    const char *getApplicationName(void) const override
    {
        return ("Controller");
    }

    const char *getApplicationVersion(void) const override
    {
        return ("3.0");
    }

    const char *getApplicationSandbox(void) const override
    {
        return ("ctrlv2");
    }

    Window *getMainWindow(void) override
    {
        return (&mainWindow);
    }

    void initialise(void) override;

private:
    static constexpr uint8_t NumPresetsInBank = 12;

    // Delegated functions ---------------------------------------------------
    void displayDefaultPage(void);

    void handleIncomingMidiMessage(const MidiInput &midiInput,
                                   const MidiMessage &midiMessage) override;
    bool handleCtrlFileReceived(LocalFile file,
                                ElectraCommand::Object fileType) override;

    // Handle preset
    void assignPresetNames(uint8_t presetBankId);
    bool loadPresetById(int presetId);
    bool loadPreset(LocalFile file);
    void reset(void);
    void runPresetLuaScript(void);

    // UI
    MainWindow mainWindow;
    UiDelegate *delegate;

    // Model
    Preset preset;

    // Midi
    Midi midi;

    // App state
    char presetNames[NumPresetsInBank][Preset::MaxNameLength + 1];
    uint8_t currentPresetBank;
    uint8_t currentPreset;
    bool readyForPresetSwitch;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
