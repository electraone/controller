#pragma once

#include "App.h"
#include "MainWindow.h"
#include "MidiInputCallback.h"
#include "MidiCallbacks.h"
#include "PageView.h"
#include "Model.h"
#include "Ui.h"
#include "UiDelegate.h"
#include "Midi.h"
#include "Api.h"
#include "MidiApi.h"
#include "Setup/Setup.h"
#include "MidiRouter.h"

UiDelegate *luaDelegate = nullptr;

class Controller : public App, private MidiInputCallback
{
public:
    Controller()
        : model(getApplicationSandbox()),
          midi(model.currentPreset),
          mainWindow(MainWindow(model, midi, appSetup)),
          delegate(mainWindow),
          api(&mainWindow),
          midiApi(appSetup.midiControls, mainWindow),
          midiRouter(appSetup.router)
    {
    }

    const char *getApplicationName(void) const override
    {
        return ("Controller");
    }

    const char *getApplicationVersion(void) const override
    {
        return ("3.0-a.10");
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
    void displayDefaultPage(void);
    void handleIncomingMidiMessage(const MidiInput &midiInput,
                                   const MidiMessage &midiMessage) override;
    bool handleCtrlFileReceived(LocalFile file,
                                ElectraCommand::Object fileType) override;
    bool handleCtrlFileRemoved(uint8_t bankNumber,
                               uint8_t slot,
                               ElectraCommand::Object fileType) override;
    void handleElectraSysex(uint8_t port,
                            const SysexBlock &sysexBlock) override;
    void handleIncomingControlMessage(MidiInput &midiInput,
                                      MidiMessage &midiMessage) override;

    // Patch requests
    void runUserTask(void);

    // Setup handling
    bool loadSetup(LocalFile file);
    bool applySetup(LocalFile file);
    void configureApp(void);

    // Model
    Model model;

    // Setup
    Setup appSetup;

    // UI
    MainWindow mainWindow;
    UiDelegate &delegate;

    // Interfaces
    Midi midi;
    Api api;
    MidiApi midiApi;

    // Midi Router
    MidiRouter midiRouter;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
