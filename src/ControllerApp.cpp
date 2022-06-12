#include "ControllerApp.h"
#include "MidiInputCallback.h"
#include "lualibs.h"

void Controller::initialise(void)
{
    // Set application context
    System::context.setAppName("ctrlv2");

    // Set delegates
    luaDelegate = delegate;

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
        delegate->switchPreset(currentPresetBank, currentPreset);
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
    if (delegate) {
        delegate->setPage(
            1, model.currentPreset.getPage(1).getDefaultControlSetId());
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
    }

    if (!model.currentPreset.isValid()) {
        logMessage("handleCtrlFileReceived: preset upload failed");
        return (false);
    }

    return (true);
}

bool Controller::handleCtrlFileRemoved(int fileNumber,
                                       ElectraCommand::Object fileType)
{
    if (fileType == ElectraCommand::Object::FilePreset) {
        // If it is a current preset, reload it
        if ((fileNumber == currentPreset) || (fileNumber == -1)) {
            logMessage("current preset");
            delegate->switchPreset(currentPresetBank, currentPreset);
        } else {
            logMessage("other preset");
        }
    } else if (fileType == ElectraCommand::Object::FileLua) {
        if ((fileNumber == currentPreset) || (fileNumber == -1)) {
            logMessage("clear Lua context");

            // disable the luaTask
            // clear Lua context
            // create a new Lua context
        }
    }
    return (true);
}

void Controller::handleElectraSysex(const SysexBlock &sysexBlock)
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
                api.sendSnapshotList(sysexPayload);
            } else {
                if (model.currentPreset.isValid()) {
                    delegate->sendSnapshotList(
                        model.currentPreset.getProjectId());
                }
            }
        } else if (object == ElectraCommand::Object::FileSnapshot) {
            api.sendSnapshot(sysexPayload);
        } else if (object == ElectraCommand::Object::PresetList) {
            api.sendPresetList();
        }
    } else if (cmd.isMidiLearnSwitch()) {
        if (object
            == ElectraCommand::Object::
                MidiLearnOff) // TODO: this is a fix for backwards compatibility
        {
            api.disableMidiLearn();
        } else {
            api.enableMidiLearn();
        }
    } else if (cmd.isSwitch()) {
        if (object == ElectraCommand::Object::PresetSlot) {
            api.switchPreset(cmd.getByte1(), cmd.getByte2());
        }
    } else if (cmd.isUpdateRuntime()) {
        if (object == ElectraCommand::Object::Control) {
            uint16_t controlId = cmd.getByte1() | cmd.getByte2() << 7;
            api.updateControl(controlId, sysexPayload);
        } else if (object == ElectraCommand::Object::SnapshotSlot) {
            api.setSnapshotSlot(sysexPayload);
        } else if (object == ElectraCommand::Object::PresetSlot) {
            api.setPresetSlot(cmd.getByte1(), cmd.getByte2());
        }
    } else if (cmd.isUpdate()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.updateSnapshot(sysexPayload);
        }
    } else if (cmd.isRemove()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.removeSnapshot(sysexPayload);
        }
    } else if (cmd.isSwap()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            api.swapSnapshots(sysexPayload);
        }
    } else if (cmd.isEvent()) {
        if (cmd.getEvent() == ElectraCommand::Event::SnapshotBankSwitch) {
            uint8_t bankNumber = cmd.getByte1();
            logMessage(
                "handleElectraSysex: snapshot bank change event : bankNumber=%d",
                bankNumber);
            api.setCurrentSnapshotBank(bankNumber);
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
