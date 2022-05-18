#pragma once

#include "App.h"
#include "MainWindow.h"
#include "MidiInputCallback.h"
#include "PageView.h"
#include "Preset.h"
#include "Ui.h"
#include "UiDelegate.h"

class Controller : public App, private MidiInputCallback
{
public:
    Controller()
        : mainWindow(MainWindow(preset)),
          delegate(nullptr),
          currentPage(0),
          currentPreset(0),
          currentPresetBank(0),
          readyForPresetSwitch(true)
    {
    }

    const char *getApplicationName(void) const override
    {
        return ("Controller");
    }

    const char *getApplicationVersion(void) const override
    {
        return ("2.3.0");
    }

    const char *getApplicationSandbox(void) const override
    {
        return ("ctrlv2");
    }

    void initialise(void) override;

    // Optional callbacks
    static void onMidiClock(MidiInput midiInput);
    static void onMidiStart(MidiInput midiInput);
    static void onMidiStop(MidiInput midiInput);
    static void onMidiContinue(MidiInput midiInput);
    static void onMidiActiveSensing(MidiInput midiInput);
    static void onMidiSystemReset(MidiInput midiInput);
    static void onMidiTuneRequest(MidiInput midiInput);
    static void onMidiProgramChange(MidiInput midiInput,
                                    uint8_t channel,
                                    uint8_t programNumber);
    static void onMidiAfterTouchChannel(MidiInput midiInput,
                                        uint8_t channel,
                                        uint8_t pressure);
    static void onMidiSongSelect(MidiInput midiInput, uint8_t songNumber);
    static void
        onMidiPitchBend(MidiInput midiInput, uint8_t channel, int value);
    static void onMidiSongPosition(MidiInput midiInput, int position);
    static void onMidiControlChange(MidiInput midiInput,
                                    uint8_t channel,
                                    uint8_t controllerNumber,
                                    uint8_t value);
    static void onMidiNoteOn(MidiInput midiInput,
                             uint8_t channel,
                             uint8_t noteNumber,
                             uint8_t velocity);
    static void onMidiNoteOff(MidiInput midiInput,
                              uint8_t channel,
                              uint8_t noteNumber,
                              uint8_t velocity);
    static void onMidiAfterTouchPoly(MidiInput midiInput,
                                     uint8_t channel,
                                     uint8_t noteNumber,
                                     uint8_t pressure);
    static void onMidiSysex(MidiInput &midiInput, SysexBlock &sysexBlock);
    static void onMidiMessage(MidiInput &midiInput, MidiMessage &midiMessage);

private:
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
    void assignLuaCallbacks(void);
    void resetMidiCallbacks(void);
    void runPresetLuaScript(void);

    // Lua support functions
    static bool luaFunctionExists(const char *module, const char *function);
    static void getModuleFunction(const char *module, const char *function);
    static void onSingleByte(const char *function, MidiInput midiInput);
    static void
        onTwoBytes(const char *function, MidiInput midiInput, int data1);
    static void onTwoBytesWithChannel(const char *function,
                                      MidiInput midiInput,
                                      uint8_t channel,
                                      int data1);
    static void onThreeBytesWithChannel(const char *function,
                                        MidiInput midiInput,
                                        uint8_t channel,
                                        uint8_t data1,
                                        uint8_t data2);

    MainWindow mainWindow;
    Preset preset;
    UiDelegate *delegate;

    char presetNames[NR_OF_PRESETS][MAX_PRESET_NAME_LENGTH + 1];

    uint8_t currentPage;
    uint8_t currentPresetBank;
    uint8_t currentPreset;
    bool readyForPresetSwitch;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
