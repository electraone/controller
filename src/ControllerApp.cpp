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

    // load the default preset
    if (System::context.getLoadDefaultFiles()) {
        LocalFile file(System::context.getCurrentPresetFile());

        if (!loadPresetById(currentPresetBank * 12 + currentPreset)) {
            logMessage("setup: preset load failed: name=%s", preset.getName());

            // Clean up the files
            Hardware::sdcard.deleteFile(System::context.getCurrentPresetFile());
            Hardware::sdcard.deleteFile(System::context.getCurrentLuaFile());
        } else {
            logMessage("setup: preset loaded: name=%s", preset.getName());

            // Execute the Lua script only if the the preset was loaded
            runPresetLuaScript();
        }

        // Log free RAM
        monitorFreeMemory();
    }

    // Set UI delegate.
    delegate = &mainWindow;

    // Display the preset if valid.
    if (preset.isValid()) {
        displayDefaultPage();
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
}

/** Incoming control file received handler.
 *  handleCtrlFileReceived () is called whenever preset file is received on
 *  Electra's control port
 */
bool Controller::handleCtrlFileReceived(LocalFile file,
                                        ElectraCommand::Object fileType)
{
    // a preset file
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
    App::get()->clearRepaintGraphics();

    // Free current preset
    reset();

    if (preset.load(presetFile) == true) {
        logMessage("Default preset loaded: filename=%s", presetFile);
    } else {
        // \todo what should be done?
        logMessage("Default preset load failed");
        return (false);
    }

    // Set UI delegate.
    delegate = &mainWindow;

    // Display the preset if valid.
    if (preset.isValid()) {
        displayDefaultPage();
    }

    return (true);
}

/** Reset preset.
 *  Re-initialize preset so that it is completely empty and ready for loading a new preset.
 */
void Controller::reset(void)
{
    // Reset Lua callbacks
    resetMidiCallbacks();

    // Reset Lua
    closeLua();

    // Reset preset
    preset.reset();

    // Reset parameterMap
    //parameterMap.reset();

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
            preset.getPresetName(file, presetNames[i], MAX_PRESET_NAME_LENGTH);
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

void Controller::assignLuaCallbacks(void)
{
    if (luaFunctionExists("midi", "onClock")) {
        logMessage("lua callback assigned: onClock");
        MidiInputCallback::onMidiClockCallback = &Controller::onMidiClock;
    }

    if (luaFunctionExists("midi", "onStart")) {
        logMessage("lua callback assigned: onStart");
        MidiInputCallback::onMidiStartCallback = &Controller::onMidiStart;
    }

    if (luaFunctionExists("midi", "onStop")) {
        logMessage("lua callback assigned: onStop");
        MidiInputCallback::onMidiStopCallback = &Controller::onMidiStop;
    }

    if (luaFunctionExists("midi", "onContinue")) {
        logMessage("lua callback assigned: onContinue");
        MidiInputCallback::onMidiContinueCallback = &Controller::onMidiContinue;
    }

    if (luaFunctionExists("midi", "onActiveSensing")) {
        logMessage("lua callback assigned: onActiveSensing");
        MidiInputCallback::onMidiActiveSensingCallback =
            &Controller::onMidiActiveSensing;
    }

    if (luaFunctionExists("midi", "onSystemReset")) {
        logMessage("lua callback assigned: onSystemReset");
        MidiInputCallback::onMidiSystemResetCallback =
            &Controller::onMidiSystemReset;
    }

    if (luaFunctionExists("midi", "onTuneRequest")) {
        logMessage("lua callback assigned: onTuneRequest");
        MidiInputCallback::onMidiTuneRequestCallback =
            &Controller::onMidiTuneRequest;
    }

    if (luaFunctionExists("midi", "onProgramChange")) {
        logMessage("lua callback assigned: onProgramChange");
        MidiInputCallback::onMidiProgramChangeCallback =
            &Controller::onMidiProgramChange;
    }

    if (luaFunctionExists("midi", "onAfterTouchChannel")) {
        logMessage("lua callback assigned: onAfterTouchChannel");
        MidiInputCallback::onMidiAfterTouchChannelCallback =
            &Controller::onMidiAfterTouchChannel;
    }

    if (luaFunctionExists("midi", "onPitchBend")) {
        logMessage("lua callback assigned: onPitchBend");
        MidiInputCallback::onMidiPitchBendCallback =
            &Controller::onMidiPitchBend;
    }

    if (luaFunctionExists("midi", "onSongSelect")) {
        logMessage("lua callback assigned: onSongSelect");
        MidiInputCallback::onMidiSongSelectCallback =
            &Controller::onMidiSongSelect;
    }

    if (luaFunctionExists("midi", "onSongPosition")) {
        logMessage("lua callback assigned: onSongPosition");
        MidiInputCallback::onMidiSongPositionCallback =
            &Controller::onMidiSongPosition;
    }

    if (luaFunctionExists("midi", "onControlChange")) {
        logMessage("lua callback assigned: onControlChange");
        MidiInputCallback::onMidiControlChangeCallback =
            &Controller::onMidiControlChange;
    }

    if (luaFunctionExists("midi", "onNoteOn")) {
        logMessage("lua callback assigned: onNoteOn");
        MidiInputCallback::onMidiNoteOnCallback = &Controller::onMidiNoteOn;
    }

    if (luaFunctionExists("midi", "onNoteOff")) {
        logMessage("lua callback assigned: onNoteOff");
        MidiInputCallback::onMidiNoteOffCallback = &Controller::onMidiNoteOff;
    }

    if (luaFunctionExists("midi", "onAfterTouchPoly")) {
        logMessage("lua callback assigned: onAfterTouchPoly");
        MidiInputCallback::onMidiAfterTouchPolyCallback =
            &Controller::onMidiAfterTouchPoly;
    }

    /*
    if (luaFunctionExists("midi", "onSysex")) {
        logMessage("lua callback assigned: onSysex");
        MidiInputCallback::onMidiSysexCallback = &Controller::onMidiSysex;
    }
    */
    /*
    if (luaFunctionExists("midi", "onMessage")) {
        logMessage("lua callback assigned: onMessage");
        MidiInputCallback::onMidiMessageCallback =
        &Controller::onMidiMessage;
    }
    */
}

void Controller::resetMidiCallbacks(void)
{
    MidiInputCallback::onMidiClockCallback = nullptr;
    MidiInputCallback::onMidiStartCallback = nullptr;
    MidiInputCallback::onMidiStopCallback = nullptr;
    MidiInputCallback::onMidiContinueCallback = nullptr;
    MidiInputCallback::onMidiActiveSensingCallback = nullptr;
    MidiInputCallback::onMidiSystemResetCallback = nullptr;
    MidiInputCallback::onMidiTuneRequestCallback = nullptr;
    MidiInputCallback::onMidiProgramChangeCallback = nullptr;
    MidiInputCallback::onMidiAfterTouchChannelCallback = nullptr;
    MidiInputCallback::onMidiPitchBendCallback = nullptr;
    MidiInputCallback::onMidiSongSelectCallback = nullptr;
    MidiInputCallback::onMidiSongPositionCallback = nullptr;
    MidiInputCallback::onMidiControlChangeCallback = nullptr;
    MidiInputCallback::onMidiNoteOnCallback = nullptr;
    MidiInputCallback::onMidiNoteOffCallback = nullptr;
    MidiInputCallback::onMidiAfterTouchPolyCallback = nullptr;
    MidiInputCallback::onMidiSysexCallback = nullptr;
    //MidiInputCallback::onMidiMessageCallback = nullptr;
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

/*
 * Optional callbacks
 */
void Controller::onMidiClock(MidiInput midiInput)
{
    onSingleByte("onClock", midiInput);
}

void Controller::onMidiStart(MidiInput midiInput)
{
    onSingleByte("onStart", midiInput);
}

void Controller::onMidiStop(MidiInput midiInput)
{
    onSingleByte("onStop", midiInput);
}

void Controller::onMidiContinue(MidiInput midiInput)
{
    onSingleByte("onContinue", midiInput);
}

void Controller::onMidiActiveSensing(MidiInput midiInput)
{
    onSingleByte("onActiveSensing", midiInput);
}

void Controller::onMidiSystemReset(MidiInput midiInput)
{
    onSingleByte("onSystemReset", midiInput);
}

void Controller::onMidiTuneRequest(MidiInput midiInput)
{
    onSingleByte("onTuneRequest", midiInput);
}

void Controller::onMidiProgramChange(MidiInput midiInput,
                                     uint8_t channel,
                                     uint8_t programNumber)
{
    onTwoBytesWithChannel("onProgramChange", midiInput, channel, programNumber);
}

void Controller::onMidiAfterTouchChannel(MidiInput midiInput,
                                         uint8_t channel,
                                         uint8_t pressure)
{
    onTwoBytesWithChannel("onAfterTouchChannel", midiInput, channel, pressure);
}

void Controller::onMidiSongSelect(MidiInput midiInput, uint8_t songNumber)
{
    onTwoBytes("onSongSelect", midiInput, songNumber);
}

void Controller::onMidiPitchBend(MidiInput midiInput,
                                 uint8_t channel,
                                 int value)
{
    onTwoBytesWithChannel("onPitchBend", midiInput, channel, value);
}

void Controller::onMidiSongPosition(MidiInput midiInput, int position)
{
    onTwoBytes("onSongPosition", midiInput, position);
}

void Controller::onMidiControlChange(MidiInput midiInput,
                                     uint8_t channel,
                                     uint8_t controllerNumber,
                                     uint8_t value)
{
    onThreeBytesWithChannel(
        "onControlChange", midiInput, channel, controllerNumber, value);
}

void Controller::onMidiNoteOn(MidiInput midiInput,
                              uint8_t channel,
                              uint8_t noteNumber,
                              uint8_t velocity)
{
    onThreeBytesWithChannel(
        "onNoteOn", midiInput, channel, noteNumber, velocity);
}

void Controller::onMidiNoteOff(MidiInput midiInput,
                               uint8_t channel,
                               uint8_t noteNumber,
                               uint8_t velocity)
{
    onThreeBytesWithChannel(
        "onNoteOff", midiInput, channel, noteNumber, velocity);
}

void Controller::onMidiAfterTouchPoly(MidiInput midiInput,
                                      uint8_t channel,
                                      uint8_t noteNumber,
                                      uint8_t pressure)
{
    onThreeBytesWithChannel(
        "onAfterTouchPoly", midiInput, channel, noteNumber, pressure);
}

void Controller::onMidiMessage(MidiInput &midiInput, MidiMessage &midiMessage)
{
    getModuleFunction("midi", "onMessage");

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "portNumber", midiInput.getPort());

        MidiMessage::Type type = midiMessage.getType();

        lua_newtable(L);
        luaLE_pushTableInteger(L, "channel", midiMessage.getChannel());
        luaLE_pushTableInteger(L, "type", (uint8_t)type);
        luaLE_pushTableInteger(L, "data1", midiMessage.getData1());
        luaLE_pushTableInteger(L, "data2", midiMessage.getData2());

        if (type == MidiMessage::Type::ControlChange) {
            luaLE_pushTableInteger(
                L, "controllerNumber", midiMessage.getData1());
            luaLE_pushTableInteger(L, "value", midiMessage.getData2());
        } else if (type == MidiMessage::Type::NoteOn) {
            luaLE_pushTableInteger(L, "noteNumber", midiMessage.getData1());
            luaLE_pushTableInteger(L, "velocity", midiMessage.getData2());
        } else if (type == MidiMessage::Type::NoteOff) {
            luaLE_pushTableInteger(L, "noteNumber", midiMessage.getData1());
            luaLE_pushTableInteger(L, "velocity", midiMessage.getData2());
        } else if (type == MidiMessage::Type::AfterTouchPoly) {
            luaLE_pushTableInteger(L, "noteNumber", midiMessage.getData1());
            luaLE_pushTableInteger(L, "pressure", midiMessage.getData2());
        } else if (type == MidiMessage::Type::ProgramChange) {
            luaLE_pushTableInteger(L, "programNumber", midiMessage.getData1());
        } else if (type == MidiMessage::Type::SongSelect) {
            luaLE_pushTableInteger(L, "songNumber", midiMessage.getData1());
        } else if (type == MidiMessage::Type::AfterTouchChannel) {
            luaLE_pushTableInteger(L, "pressure", midiMessage.getData1());
        } else if (type == MidiMessage::Type::PitchBend) {
            luaLE_pushTableInteger(L,
                                   "value",
                                   midiMessage.getData1()
                                       | midiMessage.getData2() << 7);
        } else if (type == MidiMessage::Type::SongPosition) {
            luaLE_pushTableInteger(L,
                                   "position",
                                   midiMessage.getData1()
                                       | midiMessage.getData2() << 7);
        }

        if (lua_pcall(L, 2, 0, 0) != 0) {
            logMessage("error running function 'midi.onMessage': %s",
                       lua_tostring(L, -1));
        }
    }
}

bool Controller::luaFunctionExists(const char *module, const char *function)
{
    getModuleFunction(module, function);

    if (lua_isfunction(L, -1)) {
        lua_remove(L, -1);
        return (true);
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }

    return (false);
}

void Controller::getModuleFunction(const char *module, const char *function)
{
    lua_getglobal(L, module);
    lua_pushstring(L, function);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}

void Controller::onSingleByte(const char *function, MidiInput midiInput)
{
    getModuleFunction("midi", function);

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "portNumber", midiInput.getPort());

        if (lua_pcall(L, 1, 0, 0) != 0) {
            logMessage("error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}

void Controller::onTwoBytes(const char *function,
                            MidiInput midiInput,
                            int data1)
{
    getModuleFunction("midi", function);

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "port", midiInput.getPort());
        lua_pushnumber(L, data1);

        if (lua_pcall(L, 2, 0, 0) != 0) {
            logMessage("error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}

void Controller::onTwoBytesWithChannel(const char *function,
                                       MidiInput midiInput,
                                       uint8_t channel,
                                       int data1)
{
    getModuleFunction("midi", function);

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "port", midiInput.getPort());
        lua_pushnumber(L, channel);
        lua_pushnumber(L, data1);

        if (lua_pcall(L, 3, 0, 0) != 0) {
            logMessage("error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}

void Controller::onThreeBytesWithChannel(const char *function,
                                         MidiInput midiInput,
                                         uint8_t channel,
                                         uint8_t data1,
                                         uint8_t data2)
{
    getModuleFunction("midi", function);

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "port", midiInput.getPort());
        lua_pushnumber(L, channel);
        lua_pushnumber(L, data1);
        lua_pushnumber(L, data2);

        if (lua_pcall(L, 4, 0, 0) != 0) {
            logMessage("error running function '%s': %s",
                       function,
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, function);
    }
}

void Controller::onMidiSysex(MidiInput &midiInput, SysexBlock &sysexBlock)
{
    getModuleFunction("midi", "onSysex");

    if (lua_isfunction(L, -1)) {
        lua_newtable(L);
        luaLE_pushTableInteger(
            L, "interface", (uint8_t)midiInput.getInterfaceType());
        luaLE_pushTableInteger(L, "port", midiInput.getPort());
        luaLE_pushObject(L, "SysexBlock", &sysexBlock);

        if (lua_pcall(L, 2, 0, 0) != 0) {
            logMessage("error running function 'onSysex': %s",
                       lua_tostring(L, -1));
        }
    } else {
        luaLE_handleNonexistentFunction(L, "onSysex");
    }
}
