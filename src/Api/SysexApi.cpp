#include "SysexApi.h"
#include "ArduinoJson.h"
#include "Midi/Sysex.h"

SysexApi::SysexApi(MainDelegate &newDelegate) : delegate(newDelegate)
{
}

void SysexApi::process(uint8_t port, const SysexBlock &sysexBlock)
{
    ElectraCommand cmd = sysexBlock.getElectraSysexCommand();
    MemoryBlock sysexPayload = sysexBlock.getElectraSysexPayload();
    ElectraCommand::Type command = cmd.getType();
    ElectraCommand::Object object = cmd.getObject();

    logMessage(
        "SysexApi::process: sysex received: command=%d, parameter=%d, byte1=%d",
        command,
        object,
        cmd.getByte1());

    if (cmd.isFileRequest()) {
        if (object == ElectraCommand::Object::SnapshotList) {
            sendSnapshotList(port, sysexPayload);
        } else if (object == ElectraCommand::Object::FileSnapshot) {
            sendSnapshot(port, sysexPayload);
        } else if (object == ElectraCommand::Object::PresetList) {
            sendPresetList(port);
        }
    } else if (cmd.isMidiLearnSwitch()) {
        if (object == ElectraCommand::Object::MidiLearnOff) {
            disableMidiLearn(port);
        } else {
            enableMidiLearn(port);
        }
    } else if (cmd.isSwitch()) {
        if (object == ElectraCommand::Object::PresetSlot) {
            switchPreset(port, cmd.getByte1(), cmd.getByte2());
        }
    } else if (cmd.isUpdateRuntime()) {
        if (object == ElectraCommand::Object::Control) {
            uint16_t controlId = cmd.getByte1() | cmd.getByte2() << 7;
            updateControl(port, controlId, sysexPayload);
        } else if (object == ElectraCommand::Object::SnapshotSlot) {
            setSnapshotSlot(port, sysexPayload);
        } else if (object == ElectraCommand::Object::PresetSlot) {
            setPresetSlot(port, cmd.getByte1(), cmd.getByte2());
        }
    } else if (cmd.isUpdate()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            updateSnapshot(port, sysexPayload);
        }
    } else if (cmd.isRemove()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            removeSnapshot(port, sysexPayload);
        }
    } else if (cmd.isSwap()) {
        if (object == ElectraCommand::Object::SnapshotInfo) {
            swapSnapshots(port, sysexPayload);
        }
    } else if (cmd.isEvent()) {
        if (cmd.getEvent() == ElectraCommand::Event::SnapshotBankSwitch) {
            uint8_t bankNumber = cmd.getByte1();
            logMessage(
                "SysexApi::process: snapshot bank change event : bankNumber=%d",
                bankNumber);
            setCurrentSnapshotBank(port, bankNumber);
        }
    } else if (cmd.isSystemCall()) {
        logMessage("SysexApi::process: application system call");
    } else {
        logMessage("SysexApi::process: unknown sysex request");
    }
}

void SysexApi::process(uint8_t port,
                       LocalFile &file,
                       ElectraCommand::Object fileType)
{
    if (fileType == ElectraCommand::Object::FilePreset) {
        loadPreset(port, file);
        sendPresetSlotChanged(port);
    } else if (fileType == ElectraCommand::Object::FileLua) {
        loadLua(port, file);
    } else if (fileType == ElectraCommand::Object::FileConfig) {
        loadConfig(port, file);
    } else if (fileType == ElectraCommand::Object::FileSnapshot) {
        importSnapshot(port, file);
    }
}

void SysexApi::sendSnapshotList(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::sendSnapshotList");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("sendSnapshotList: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();

    if (projectId) {
        delegate.sendSnapshotList(port, projectId);
    }
}

void SysexApi::sendSnapshot(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::sendSnapshot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("sendSnapshot: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();

    delegate.sendSnapshot(port, projectId, bankNumber, slot);
}

void SysexApi::sendPresetList(uint8_t port) // \todo port does not belong here
{
    logMessage("SysexApi::sendPresetList");
    delegate.sendPresetList(port);
}

void SysexApi::enableMidiLearn(uint8_t port)
{
    logMessage("SysexApi::enableMidiLearn");
    delegate.enableMidiLearn();
    sendAck(port);
}

void SysexApi::disableMidiLearn(uint8_t port)
{
    logMessage("SysexApi::disableMidiLearn");
    delegate.disableMidiLearn();
    sendAck(port);
}

void SysexApi::switchPreset(uint8_t port, uint8_t bankNumber, uint8_t slot)
{
    logMessage("SysexApi::switchPreset");
    delegate.switchPreset(bankNumber, slot);
    sendPresetSwitched(port, bankNumber, slot);
    sendAck(port);
}

void SysexApi::updateControl(uint8_t port,
                             uint16_t controlId,
                             MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::updateControl");
    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    // skip first two bytes (after command and object)
    sysexPayload.read();
    sysexPayload.read();

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("updateControl: control info parsing failed");
        sendNack(port);
        return;
    }

    if (!doc["name"].isNull()) {
        const char *name = doc["name"].as<char *>();
        delegate.setControlName(controlId, name);
    }

    if (!doc["color"].isNull()) {
        uint8_t colour =
            ElectraColours::translateColour(doc["color"].as<char *>());
        delegate.setControlColour(controlId, colour);
    }

    if (!doc["visible"].isNull()) {
        bool shouldBeVisible = doc["visible"].as<bool>();
        delegate.setControlVisible(controlId, shouldBeVisible);
    }
    sendAck(port);
}

void SysexApi::setSnapshotSlot(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::setSnapshotSlot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("setSnapshotSlot: snapshot info parsing failed");
        sendNack(port);
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();

    delegate.setSnapshotSlot(projectId, bankNumber, slot);
    sendAck(port);
}

bool SysexApi::loadPreset(uint8_t port, LocalFile &file)
{
    return (delegate.loadPreset(file));
}

bool SysexApi::loadLua(uint8_t port, LocalFile &file)
{
    return (delegate.loadLua(file));
}

bool SysexApi::loadConfig(uint8_t port, LocalFile &file)
{
    // this is commented out on purpose. We do not want to load the config here yet.
    //delegate.loadConfig(file);
    //sendAck(port);
    return (true);
}

bool SysexApi::importSnapshot(uint8_t port, LocalFile &file)
{
    return (delegate.importSnapshot(file));
}

void SysexApi::setPresetSlot(uint8_t port, uint8_t bankNumber, uint8_t slot)
{
    logMessage("SysexApi::setPresetSlot");
    delegate.setPresetSlot(bankNumber, slot);
    sendAck(port);
}

void SysexApi::updateSnapshot(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::updateSnapshot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("updateSnapshot: snapshot info parsing failed");
        sendNack(port);
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();
    const char *name = doc["name"].as<char *>();
    Colour colour = ElectraColours::translateColour(doc["color"].as<char *>());

    delegate.updateSnapshot(projectId, bankNumber, slot, name, colour);
    sendAck(port);
}

void SysexApi::removeSnapshot(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("SysexApi::removeSnapshot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("removeSnapshot: snapshot info parsing failed");
        sendNack(port);
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();

    delegate.removeSnapshot(projectId, bankNumber, slot);
    sendAck(port);
}

void SysexApi::swapSnapshots(uint8_t port, MemoryBlock &sysexPayload)
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("swapSnapshots: snapshot info parsing failed");
        sendNack(port);
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t sourceBankNumber = doc["fromBankNumber"].as<uint8_t>();
    uint8_t sourceSlot = doc["fromSlot"].as<uint8_t>();
    uint8_t destBankNumber = doc["toBankNumber"].as<uint8_t>();
    uint8_t destSlot = doc["toSlot"].as<uint8_t>();

    delegate.swapSnapshots(
        projectId, sourceBankNumber, sourceSlot, destBankNumber, destSlot);
    sendAck(port);
}

void SysexApi::setCurrentSnapshotBank(uint8_t port, uint8_t bankNumber)
{
    delegate.setCurrentSnapshotBank(bankNumber);
    sendAck(port);
}