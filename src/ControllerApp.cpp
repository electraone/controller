#include "ControllerApp.h"
#include "MidiInputCallback.h"
#include "lualibs.h"

void Controller::initialise(void)
{
    // Set application context
    System::context.setAppName("ctrlv2");

    // Get info about the last used preset
    currentPreset = System::runtimeInfo.getLastActivePreset();
    System::context.setCurrentFile(currentPreset);

    // Load application setup file
    LocalFile config(System::context.getCurrentConfigFile());

    // Special boot - do not read the default preset
    if (Hardware::buttons[BUTTON_LEFT_TOP]->process() == true) {
        logMessage("Special boot mode: do not read preset");
        System::context.setLoadDefaultFiles(false);
    } else if (Hardware::buttons[BUTTON_LEFT_MIDDLE]->process() == true) {
        //removeAllAppFiles();
        System::runtimeInfo.setLastActivePreset(0);
    } else if (Hardware::buttons[BUTTON_LEFT_BOTTOM]->process() == true) {
        logMessage("Special boot: do not use knob touch");
        System::context.setTouchEnabled(false);
        //systemTasks.doNotUsePotTouch();
    }

    // Log free RAM
    monitorFreeMemory();

    // Read preset names and assign them to preset seletion knobs
    assignPresetNames(currentPresetBank);

    // Register ParameterMap onChange callback
    parameterMap.onChange = [this](LookupEntry *entry, Origin origin) {
        if (origin != Origin::midi) {
            // \todo taking the first message destination might not be ok
            // in some situations
            Message message = entry->messageDestination[0].value->message;
            message.setValue(entry->midiValue);
            midi.sendMessage(message);
        }

        // Execute only if Lua is loaded
        if (L && entry) {
            parameterMap_onChange(entry, origin);
        }
    };

    // load the default preset
    if (System::context.getLoadDefaultFiles()) {
        if (!loadPresetById(currentPresetBank * 12 + currentPreset)) {
            logMessage("setup: preset load failed: name=%s", preset.getName());

        } else {
            logMessage("setup: preset loaded: name=%s", preset.getName());
        }

        // Log free RAM
        monitorFreeMemory();
    }

    // Set UI delegate.
    delegate = &mainWindow;

    // Display the preset if valid.
    if (preset.isValid()) {
        displayDefaultPage();
        //preset.print();
    }

    // Finalise the initialisation
    logMessage("App initialisation completed");

    // Set logger status according to the saved configuration
    loggerEnabled = System::runtimeInfo.getLoggerStatus();
}

void Controller::displayDefaultPage(void)
{
    if (delegate) {
        delegate->setPage(0);
    }
}

/** Incoming MIDI message handler.
 *  handleIncomingMidiMessage () is called when a MIDI message arrives to Electra's
 *  MIDI ports (excluding Control port)
 */
void Controller::handleIncomingMidiMessage(const MidiInput &midiInput,
                                           const MidiMessage &midiMessage)
{
    if (System::context.getMidiLearn()) {
        //midi.processMidiLearn(device, midiInput, midiMessage);
    } else {
        midi.processMidi(midiInput, midiMessage);
    }
}

/** Incoming control file received handler.
 *  handleCtrlFileReceived () is called whenever preset file is received on
 *  Electra's control port
 */
bool Controller::handleCtrlFileReceived(LocalFile file,
                                        ElectraCommand::Object fileType)
{
    if (fileType == ElectraCommand::Object::FilePreset) {
        // read preset names and assign them to preset seletion knobs
        assignPresetNames(currentPresetBank);

        // load a new preset
        if (loadPreset(file)) {
            logMessage("handleCtrlFileReceived: preset loaded: name=%s",
                       preset.getName());

            // Loading the preset always (temporary fix) removes the Lua script
            // ie. the Lua script needs to be transferred afterwards
            Hardware::sdcard.deleteFile(System::context.getCurrentLuaFile());
        }
    } else if (fileType == ElectraCommand::Object::FileLua) {
        if (isLuaValid(System::context.getCurrentLuaFile())) {
            if (preset.isValid()) {
                runPresetLuaScript();
            }
        }
    }

    if (!preset.isValid()) {
        logMessage("handleCtrlFileReceived: preset upload failed");
        return (false);
    }

    return (true);
}

/** Load preset.
 *  Read preset file and initialize all data structures to display and run the preset.
 */
bool Controller::loadPreset(LocalFile file)
{
    const char *presetFile = file.getFilepath();

    // clear all entries in the frame buffer
    System::tasks.clearRepaintGraphics();

    // Free current preset
    reset();

    if (preset.load(presetFile) == true) {
        logMessage("Default preset loaded: filename=%s", presetFile);
    } else {
        // \todo what should be done?
        logMessage("Default preset load failed");
        return (false);
    }

    //electraMidi.registerLuaFunctions(&preset.luaFunctions);

    // Set UI delegate.
    delegate = &mainWindow;

    // Display the preset if valid.
    if (preset.isValid()) {
        // Initialise the parameterMap
        for (auto &[id, control] : preset.controls) {
            for (auto &value : control.values) {
                MessageDestination messageDestination(&control, &value);

                if (value.message.getType() == ElectraMessageType::start) {
                    parameterMap.getOrCreate(0xff, value.message.getType(), 0)
                        ->messageDestination.push_back(messageDestination);
                } else if (value.message.getType()
                           == ElectraMessageType::stop) {
                    parameterMap.getOrCreate(0xff, value.message.getType(), 0)
                        ->messageDestination.push_back(messageDestination);
                } else if (value.message.getType()
                           == ElectraMessageType::tune) {
                    parameterMap.getOrCreate(0xff, value.message.getType(), 0)
                        ->messageDestination.push_back(messageDestination);
                } else {
                    LookupEntry *lookupEntry = parameterMap.getOrCreate(
                        value.message.getDeviceId(),
                        value.message.getType(),
                        value.message.getParameterNumber());

                    lookupEntry->messageDestination.push_back(
                        messageDestination);

                    int16_t midiValue = 0;

                    if (control.getType() == ControlType::pad) {
                        midiValue = value.getDefault();
                    } else {
                        midiValue = translateValueToMidiValue(
                            value.message.getSignMode(),
                            value.message.getBitWidth(),
                            value.getDefault(),
                            value.getMin(),
                            value.getMax(),
                            value.message.getMidiMin(),
                            value.message.getMidiMax());
                    }
                    parameterMap.setValue(value.message.getDeviceId(),
                                          value.message.getType(),
                                          value.message.getParameterNumber(),
                                          midiValue,
                                          Origin::internal);
                }
            }
        }

        preset.print();
        parameterMap.print();

        displayDefaultPage();
    }

    parameterMap.enable();

    return (true);
}

/** Reset preset.
 *  Re-initialize preset so that it is completely empty and ready for loading a new preset.
 */
void Controller::reset(void)
{
    // Disable the ParameterMap sync
    parameterMap.disable();

    // Reset Lua callbacks
    resetMidiCallbacks();

    // Reset Lua
    closeLua();

    // Reset preset
    preset.reset();

    // Reset parameterMap
    parameterMap.reset();

    // Clear all windows
    /*for (uint i = 0; i < MaxNumberOfPageWindows; i++) {
        pages[i].removeAllChildren();
        pages[i].removePots();
    }*/

    logMessage("Controller::reset: preset memory deallocated");
    monitorFreeMemory();
}

/** Read names of available presets from stored preset files
 *
 */
void Controller::assignPresetNames(uint8_t presetBankId)
{
    for (uint16_t i = 0; i < 12; i++) {
        char filename[MAX_FILENAME_LENGTH + 1];
        snprintf(filename,
                 MAX_FILENAME_LENGTH,
                 "%s/p%03d.epr",
                 getApplicationSandbox(),
                 (presetBankId * 12) + i);

        File file = Hardware::sdcard.createInputStream(filename);

        if (file) {
            preset.getPresetName(file, presetNames[i], Preset::MaxNameLength);
            logMessage("preset name: %s", presetNames[i]);
            file.close();
        } else {
            *presetNames[i] = '\0';
        }
    }
}

/** Load preset identified with a preset Id
 *
 */
bool Controller::loadPresetById(int presetId)
{
    if (!readyForPresetSwitch) {
        logMessage(
            "Controller::loadPresetById: still busy with swicthing previous preset");
        return (false);
    } else {
        readyForPresetSwitch = false;
    }

    if (presetId > 71) {
        presetId = 0;
    }

    currentPreset = presetId;
    System::context.setCurrentFile(presetId);
    LocalFile file(System::context.getCurrentPresetFile());

    // Remember the preset for the next startup
    System::runtimeInfo.setLastActivePreset(presetId);

    // Try to load the preset
    if (loadPreset(file)) {
        logMessage("loadPresetById: preset loaded: name=%s", preset.getName());

        // Re-set Lua state and execute
        runPresetLuaScript();
    } else {
        logMessage("loadPresetById: preset loading failed");
        //bottomBar.update(preset.getName(), preset.pages[0].getName());
        currentPage = 0;
    }

    //sendPresetSwitch(presetId / 12, presetId % 12);
    readyForPresetSwitch = true;

    return (true);
}

/** Run lua script
 *
 */
void Controller::runPresetLuaScript(void)
{
    closeLua();

    if (isLuaValid(System::context.getCurrentLuaFile())) {
        initLua();
        loadLuaLibs();

        executeElectraLua(System::context.getCurrentLuaFile());

        // assign Lua callbacks
        assignLuaCallbacks();
    }
}
