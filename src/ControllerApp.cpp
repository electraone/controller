#include "ControllerApp.h"
#include "MidiInputCallback.h"
#include "lualibs.h"

void Controller::initialise(void)
{
    // Set application context
    System::context.setAppName("ctrlv2");

    // Set delegates
    luaDelegate = &delegate;

    // Get info about the last used preset
    uint8_t presetId = System::runtimeInfo.getLastActivePreset();

    // Load application setup file
    LocalFile config(System::context.getCurrentConfigFile());

    if (!loadSetup(config)) {
        appSetup.useDefault();
    }

    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 0, appSetup.router.midiIo1Thru);
    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 1, appSetup.router.midiIo2Thru);

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
        delegate.switchPreset(presetId / 12, presetId % 12);
    }

    // Log free RAM
    monitorFreeMemory();

    // Finalise the initialisation
    logMessage("App initialisation completed");

    // Set logger status according to the saved configuration
    loggerEnabled = System::runtimeInfo.getLoggerStatus();
}

void Controller::displayDefaultPage(void)
{
    delegate.closeDetail();
    delegate.setPage(1,
                     model.currentPreset.getPage(1).getDefaultControlSetId());
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
        if (model.presets.loadPreset(file)) {
            logMessage("handleCtrlFileReceived: preset loaded: name=%s",
                       model.currentPreset.getName());

            // Loading the preset always (temporary fix) removes the Lua script
            // ie. the Lua script needs to be transferred afterwards
            Hardware::sdcard.deleteFile(System::context.getCurrentLuaFile());
            displayDefaultPage();
        }
    } else if (fileType == ElectraCommand::Object::FileLua) {
        if (isLuaValid(System::context.getCurrentLuaFile())) {
            if (model.currentPreset.isValid()) {
                model.presets.runPresetLuaScript();
            }
        }
    } else if (fileType == ElectraCommand::Object::FileConfig) {
        LocalFile config(System::context.getCurrentConfigFile());
        applySetup(file);
        loadSetup(config);
    }

    if (!model.currentPreset.isValid()) {
        logMessage("handleCtrlFileReceived: preset upload failed");
        return (false);
    }

    return (true);
}

bool Controller::handleCtrlFileRemoved(uint8_t bankNumber,
                                       uint8_t slot,
                                       ElectraCommand::Object fileType)
{
    uint8_t currentPreset = model.presets.getCurrentSlot();
    uint8_t currentPresetBank = model.presets.getCurrentBankNumber();

    if (fileType == ElectraCommand::Object::FilePreset) {
        // If it is a current preset, reload it
        if ((currentPresetBank == bankNumber) || (currentPreset == slot)) {
            delegate.switchPreset(currentPresetBank, currentPreset);
        }
    } else if (fileType == ElectraCommand::Object::FileLua) {
        if ((currentPresetBank == bankNumber) || (currentPreset == slot)) {
            logMessage("clear Lua context");

            // disable the luaTask
            // clear Lua context
            // create a new Lua context
        }
    }
    return (true);
}

void Controller::handleElectraSysex(uint8_t port, const SysexBlock &sysexBlock)
{
    ElectraCommand cmd = sysexBlock.getElectraSysexCommand();
    MemoryBlock sysexPayload = sysexBlock.getElectraSysexPayload();
    ElectraCommand::Type command = cmd.getType();
    ElectraCommand::Object object = cmd.getObject();

    logMessage(
        "handleElectraSysex: sysex received: command=%d, parameter=%d, byte1=%d",
        command,
        object,
        cmd.getByte1());

    if (cmd.isFileRequest()) {
        if (object == ElectraCommand::Object::SnapshotList) {
            if (cmd.getByte1() != 0xF7) {
                api.sendSnapshotList(port, sysexPayload);
            } else {
                if (model.currentPreset.isValid()) {
                    delegate.sendSnapshotList(
                        port, model.currentPreset.getProjectId());
                }
            }
        } else if (object == ElectraCommand::Object::FileSnapshot) {
            api.sendSnapshot(port, sysexPayload);
        } else if (object == ElectraCommand::Object::PresetList) {
            api.sendPresetList(port);
        }
    } else if (cmd.isMidiLearnSwitch()) {
        if (object
            == ElectraCommand::Object::
                MidiLearnOff) // TODO: this is a fix for backwards compatibility
        {
            api.disableMidiLearn();
            sendAck(port);
        } else {
            api.enableMidiLearn();
            sendAck(port);
        }
    } else if (cmd.isSwitch()) {
        if (object == ElectraCommand::Object::PresetSlot) {
            api.switchPreset(cmd.getByte1(), cmd.getByte2());
            sendAck(port);
        }
    } else if (cmd.isUpdateRuntime()) {
        if (object == ElectraCommand::Object::Control) {
            uint16_t controlId = cmd.getByte1() | cmd.getByte2() << 7;
            api.updateControl(controlId, sysexPayload);
            sendAck(port);
        } else if (object == ElectraCommand::Object::SnapshotSlot) {
            api.setSnapshotSlot(sysexPayload);
            sendAck(port);
        } else if (object == ElectraCommand::Object::PresetSlot) {
            api.setPresetSlot(cmd.getByte1(), cmd.getByte2());
            sendAck(port);
        }
    } else if (cmd.isUpdate()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.updateSnapshot(sysexPayload);
            sendAck(port);
        }
    } else if (cmd.isRemove()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.removeSnapshot(sysexPayload);
            sendAck(port);
        }
    } else if (cmd.isSwap()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.swapSnapshots(sysexPayload);
            sendAck(port);
        }
    } else if (cmd.isEvent()) {
        if (cmd.getEvent() == ElectraCommand::Event::SnapshotBankSwitch) {
            uint8_t bankNumber = cmd.getByte1();
            logMessage(
                "handleElectraSysex: snapshot bank change event : bankNumber=%d",
                bankNumber);
            api.setCurrentSnapshotBank(bankNumber);
            sendAck(port);
        }
    } else if (cmd.isSystemCall()) {
        logMessage("handleElectraSysex: application system call");
    } else {
        logMessage("handleElectraSysex: unknown sysex request");
    }
}

void Controller::runUserTask(void)
{
    PatchRequest request;

    if (patchRequests.isEmpty() != true) {
        request = patchRequests.shift();
        Device device = model.currentPreset.getDevice(request.deviceId);

        if (device.isValid()) {
            midi.sendTemplatedSysex(device, request.data);
        }
    } else {
        System::tasks.disableUserTask();
    }
}

/** Apply setup change.
 *
 */
bool Controller::applySetup(LocalFile file)
{
    bool success = false;

    if (appSetup.load(file.getFilepath())) {
        appSetup.serialize();
        success = true;

        configureApp();
    }

    return (success);
}

/** Load setup.
 *
 */
bool Controller::loadSetup(LocalFile file)
{
    bool success = false;

    if (appSetup.load(file.getFilepath())) {
        success = true;
        logMessage("setup loaded");
        configureApp();
    }

    return (success);
}

void Controller::configureApp(void)
{
    //  assignUSBdevicesToPorts();

    delegate.setActiveControlSetType(appSetup.uiFeatures.activeControlSetType);

    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 0, appSetup.router.midiIo1Thru);
    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 1, appSetup.router.midiIo2Thru);
}
