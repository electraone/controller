#include "ControllerApp.h"
#include "lualibs.h"

void Controller::initialise(void)
{
    // Set application context
    System::context.setAppName("ctrlv2");

    // Set delegates
    luaDelegate = &delegate;

    // Get info about the last used preset
    uint8_t presetId = System::runtimeInfo.getLastActivePreset();

    // Load Configuration or use defaults
    if (!loadConfig()) {
        appConfig.useDefault();
    }

    // Special boot - do not read the default preset
    if (Hardware::buttons[BUTTON_LEFT_TOP]->process() == true) {
        logMessage("Special boot mode: do not read preset");
        System::context.setLoadDefaultFiles(false);
    } else if (Hardware::buttons[BUTTON_LEFT_MIDDLE]->process() == true) {
        Hardware::sdcard.format();
        System::runtimeInfo.setLastActivePreset(0);
    } else if (Hardware::buttons[BUTTON_LEFT_BOTTOM]->process() == true) {
        logMessage("Special boot: do not use knob touch");
        System::context.setTouchEnabled(false);
        //systemTasks.doNotUsePotTouch();
    }

    // Log free RAM (before the preset is loaded)
    monitorFreeMemory();

    // Let parameterMap know what the app sandbox is
    parameterMap.setAppSandbox(getApplicationSandbox());

    // Register ParameterMap onChange callback
    parameterMap.onChange = [this](LookupEntry *entry, Origin origin) {
        if (origin != Origin::midi && origin != Origin::file) {
            // \todo taking the first message destination might not be ok
            // in some situations
            Message message = entry->messageDestination[0]->message;
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
    } else {
        delegate.initialiseEmpty();
    }

    // Log free RAM (after the preset is loaded and the default page displayed)
    monitorFreeMemory();

    // Finalise the initialisation
    logMessage("App initialisation completed");

    // Set logger status according to the saved configuration
    loggerEnabled = System::runtimeInfo.getLoggerStatus();
}

/** Incoming MIDI message handler.
 *  handleIncomingMidiMessage () is called when a MIDI message arrives to Electra's
 *  MIDI ports (excluding Control port)
 */
void Controller::handleIncomingMidiMessage(const MidiInput &midiInput,
                                           const MidiMessage &midiMessage)
{
    if ((appConfig.router.usbDevToMidiControl == true)
        && (midiInput.getPort() == appConfig.router.midiControlPort)
        && (midiMessage.getChannel()
            == appConfig.router.midiControlChannel)) {
        midiApi.process(midiMessage);
        if (appConfig.router.midiControlDrop) {
            return;
        }
    }

    if (System::context.getMidiLearn()) {
        midiLearn.process(midiInput, midiMessage);
    } else {
        midi.process(midiInput, midiMessage);
    }
}

/** Incoming file received handler.
 *  handleCtrlFileReceived () is called whenever file is received using
 *  the Electra upload SysEx command.
 */
bool Controller::handleCtrlFileReceived(uint8_t port,
                                        LocalFile file,
                                        ElectraCommand::Object fileType)
{
    if (fileType == ElectraCommand::Object::FileConfig) {
        applyChangesToConfig(file);
    } else {
        sysexApi.process(port, file, fileType);
    }

    return (true);
}

/** File removed handler.
 *  handleCtrlFileRemoved() is called whenever file is removed using
 *  the Electra upload SysEx command.
 */
bool Controller::handleCtrlFileRemoved(uint8_t bankNumber,
                                       uint8_t slot,
                                       ElectraCommand::Object fileType)
{
    uint8_t currentPreset = model.presets.getCurrentSlot();
    uint8_t currentPresetBank = model.presets.getCurrentBankNumber();

    if (fileType == ElectraCommand::Object::FilePreset) {
        // If it is a current preset, reload it
        if ((currentPresetBank == bankNumber) && (currentPreset == slot)) {
            delegate.switchPreset(currentPresetBank, currentPreset);
        }
    }

    return (true);
}

/** Electra SysEx handler
 *
 */
void Controller::handleElectraSysex(uint8_t port, const SysexBlock &sysexBlock)
{
    sysexApi.process(port, sysexBlock);
}

/** Electra External Midi control message handler.
 *
 */
void Controller::handleIncomingControlMessage(MidiInput &midiInput,
                                              MidiMessage &midiMessage)
{
    midiApi.process(midiMessage);
}

/** User configurable task
 * Currently misused for sending Patch request messages.
 */
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

/** Apply configuation change.
 *
 * Loads config from a (temp) file, merges it with existing, and writes
 * the new config file to the standard location
 */
bool Controller::applyChangesToConfig(LocalFile file)
{
    if (appConfig.load(file.getFilepath())) {
        appConfig.serialize();
        if (loadConfig()) {
            delegate.displayPage();
            return (true);
        }
    }
    return (false);
}

/** Load configuration.
 *
 */
bool Controller::loadConfig(void)
{
    if (appConfig.load()) {
        configureApp();
        return (true);
    }
    return (false);
}

/** Reconfigure Electra at run-time
 *
 */
void Controller::configureApp(void)
{
    delegate.setActiveControlSetType(appConfig.uiFeatures.activeControlSetType);

    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 0, appConfig.router.midiIo1Thru);
    MidiOutput::enableThru(
        MidiInterface::Type::MidiIo, 1, appConfig.router.midiIo2Thru);
}

/** USB Host port assigment callback
 * allback to determine assigment of USB device to USB Host ports.
 */
uint8_t Controller::getUsbHostPortAssigment(const char *productName)
{
    return appConfig.getUsbHostAssigment(productName);
}
