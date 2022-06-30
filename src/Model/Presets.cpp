#include "Presets.h"
#include "Midi/Sysex.h"
#include "MidiCallbacks.h"
#include "lualibs.h"
#include "luabridge.h"

Preset *luaPreset = nullptr;

Presets::Presets(const char *newAppSandbox)
    : appSandbox(newAppSandbox),
      currentSlot(0),
      currentBankNumber(0),
      readyForPresetSwitch(true)
{
    snprintf(tempFilename, MAX_FILENAME_LENGTH, "%s/eprxfer.tmp", appSandbox);
}

void Presets::assignPresetNames(uint8_t bankNumber)
{
    for (uint16_t i = 0; i < NumPresetsInBank; i++) {
        char filename[MAX_FILENAME_LENGTH + 1];
        snprintf(filename,
                 MAX_FILENAME_LENGTH,
                 "%s/p%03d.epr",
                 appSandbox,
                 (bankNumber * NumPresetsInBank) + i);

        if (File file = Hardware::sdcard.createInputStream(filename)) {
            Preset::getPresetName(file, presetNames[i], Preset::MaxNameLength);
            file.close();
        } else {
            *presetNames[i] = '\0';
        }
    }
}

void Presets::sendList(uint8_t port)
{
    File presetListFile = Hardware::sdcard.createOutputStream(
        tempFilename, FILE_WRITE | O_CREAT | O_TRUNC);

    if (!presetListFile) {
        logMessage("Presets::sendList: cannot open transfer file");
    }

    bool firstRecord = true;
    char buf[256]; // TODO: fix buffer overflow issue

    presetListFile.print("[");

    for (uint8_t i = 0; i < 72; i++) {
        char filename[MAX_FILENAME_LENGTH + 1];
        char presetName[Preset::MaxNameLength + 1];
        char projectId[Preset::MaxProjectIdLength + 1];

        snprintf(filename, MAX_FILENAME_LENGTH, "%s/p%03d.epr", appSandbox, i);

        *presetName = '\0';
        *projectId = '\0';

        if (Hardware::sdcard.exists(filename)) {
            File file = Hardware::sdcard.createInputStream(filename);

            if (file) {
                Preset::getPresetName(file, presetName, Preset::MaxNameLength);
                Preset::getPresetProjectId(
                    file, projectId, Preset::MaxProjectIdLength);
            }
            file.close();
        }

        if (strlen(presetName) > 0) {
            sprintf(
                buf,
                "%s{\"slot\":%d,\"bankNumber\":%d,\"name\":\"%s\",\"projectId\":\"%s\"}",
                (firstRecord) ? "" : ",",
                i % 12,
                i / 12,
                presetName,
                projectId);
            presetListFile.write(buf, strlen(buf));
            firstRecord = false;
        }
    }

    presetListFile.print("]");
    presetListFile.close();

    sendSysExFile(port, tempFilename, ElectraCommand::Object::PresetList);
}

/** Load preset.
 *  Read preset file and initialize all data structures to display and run the preset.
 */
bool Presets::loadPreset(LocalFile file)
{
    const char *presetFile = file.getFilepath();

    // clear all entries in the frame buffer
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();

    // Free current preset
    reset();

    if (preset.load(presetFile) == true) {
        logMessage("Default preset loaded: filename=%s", presetFile);
    } else {
        // \todo what should be done?
        logMessage("Default preset load failed");
    }

    // Display the preset if valid.
    if (preset.isValid()) {
        // Initialise the parameterMap
        for (auto &[id, control] : preset.controls) {
            for (auto &value : control.values) {
                control.setDefaultValue(value);
            }
        }

        //        snapshots.setProjectId(preset.getProjectId());
        //preset.print();
        //parameterMap.print();

        //        displayDefaultPage();
    }

    System::tasks.enableRepaintGraphics();
    parameterMap.enable();
    assignPresetNames(currentBankNumber);

    return (true);
}

/** Reset preset.
 *  Re-initialize preset so that it is completely empty and ready for loading a new preset.
 */
void Presets::reset(void)
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

    logMessage("Controller::reset: preset memory deallocated");
    monitorFreeMemory();
}

/** Load preset identified with a preset Id
 *
 */
bool Presets::loadPresetById(int presetId)
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

    setBankNumberAndSlot(presetId);
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
    }

    //sendPresetSwitch(presetId / 12, presetId % 12);
    readyForPresetSwitch = true;

    return (true);
}

/** Run lua script
 *
 */
void Presets::runPresetLuaScript(void)
{
    closeLua();

    luaPreset = &preset;

    if (isLuaValid(System::context.getCurrentLuaFile())) {
        initLua();
        loadLuaLibs();

        executeElectraLua(System::context.getCurrentLuaFile());

        // assign Lua callbacks
        assignLuaCallbacks();
    }
}

void Presets::setBankNumberAndSlot(uint8_t presetId)
{
    currentBankNumber = presetId / NumPresetsInBank;
    currentSlot = presetId % NumPresetsInBank;
    setDefaultFiles(currentBankNumber, currentSlot);
}

void Presets::setBankNumberAndSlot(uint8_t newBankNumber, uint8_t newSlot)
{
    currentBankNumber = newBankNumber;
    currentSlot = newSlot;
    setDefaultFiles(currentBankNumber, currentSlot);
}

uint8_t Presets::getPresetId(void) const
{
    return (currentBankNumber * NumPresetsInBank + currentSlot);
}

void Presets::setCurrentSlot(uint8_t newSlot)
{
    currentSlot = newSlot;
    setDefaultFiles(currentBankNumber, currentSlot);
}

uint8_t Presets::getCurrentSlot(void) const
{
    return (currentSlot);
}

void Presets::setCurrentBankNumber(uint8_t newBankNumber)
{
    currentBankNumber = newBankNumber;
    setDefaultFiles(currentBankNumber, currentSlot);
    assignPresetNames(currentBankNumber);
}

uint8_t Presets::getCurrentBankNumber(void) const
{
    return (currentBankNumber);
}

void Presets::setDefaultFiles(uint8_t newBankNumber, uint8_t newSlot)
{
    System::context.setCurrentFile(newBankNumber * NumPresetsInBank + newSlot);
}

const char *Presets::getPresetName(uint8_t slotId) const
{
    return presetNames[slotId];
}
