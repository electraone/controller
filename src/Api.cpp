#include "Api.h"
#include "ArduinoJson.h"
#include "Midi/Sysex.h"

void Api::sendSnapshotList(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("Api::sendSnapshotList");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("sendSnapshotList: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();

    delegate->sendSnapshotList(port, projectId);
}

void Api::sendSnapshot(uint8_t port, MemoryBlock &sysexPayload)
{
    logMessage("Api::sendSnapshot");

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

    delegate->sendSnapshot(port, projectId, bankNumber, slot);
}

void Api::sendPresetList(uint8_t port) // \todo port does not belong here
{
    logMessage("Api::sendPresetList");
    delegate->sendPresetList(port);
}

void Api::enableMidiLearn(void)
{
    logMessage("Api::enableMidiLearn");
    delegate->enableMidiLearn();
}

void Api::disableMidiLearn(void)
{
    logMessage("Api::disableMidiLearn");
    delegate->disableMidiLearn();
}

void Api::switchPreset(uint8_t bankNumber, uint8_t slot)
{
    logMessage("Api::switchPreset");
    delegate->switchPreset(bankNumber, slot);
}

void Api::updateControl(uint16_t controlId, MemoryBlock &sysexPayload)
{
    logMessage("Api::updateControl");
    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    // skip first two bytes (after command and object)
    sysexPayload.read();
    sysexPayload.read();

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("updateControl: control info parsing failed");
        return;
    }

    if (!doc["name"].isNull()) {
        const char *name = doc["name"].as<char *>();
        delegate->setControlName(controlId, name);
    }

    if (!doc["color"].isNull()) {
        uint8_t colour =
            ElectraColours::translateColour(doc["color"].as<char *>());
        delegate->setControlColour(controlId, colour);
    }

    if (!doc["visible"].isNull()) {
        bool shouldBeVisible = doc["visible"].as<bool>();
        delegate->setControlVisible(controlId, shouldBeVisible);
    }
}

void Api::setSnapshotSlot(MemoryBlock &sysexPayload)
{
    logMessage("Api::setSnapshotSlot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("setSnapshotSlot: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();

    delegate->setSnapshotSlot(projectId, bankNumber, slot);
}

void Api::setPresetSlot(uint8_t bankNumber, uint8_t slot)
{
    logMessage("Api::setPresetSlot");
    delegate->setPresetSlot(bankNumber, slot);
}

void Api::updateSnapshot(MemoryBlock &sysexPayload)
{
    logMessage("Api::updateSnapshot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("updateSnapshot: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();
    const char *name = doc["name"].as<char *>();
    Colour colour = ElectraColours::translateColour(doc["color"].as<char *>());

    delegate->updateSnapshot(projectId, bankNumber, slot, name, colour);
}

void Api::removeSnapshot(MemoryBlock &sysexPayload)
{
    logMessage("Api::removeSnapshot");

    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("removeSnapshot: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t bankNumber = doc["bankNumber"].as<uint8_t>();
    uint8_t slot = doc["slot"].as<uint8_t>();

    delegate->removeSnapshot(projectId, bankNumber, slot);
}

void Api::swapSnapshots(MemoryBlock &sysexPayload)
{
    const size_t capacity = JSON_OBJECT_SIZE(1) + 500;
    StaticJsonDocument<capacity> doc;

    DeserializationError err = deserializeJson(doc, sysexPayload);

    if (err) {
        logMessage("swapSnapshots: snapshot info parsing failed");
        return;
    }

    const char *projectId = doc["projectId"].as<char *>();
    uint8_t sourceBankNumber = doc["fromBankNumber"].as<uint8_t>();
    uint8_t sourceSlot = doc["fromSlot"].as<uint8_t>();
    uint8_t destBankNumber = doc["toBankNumber"].as<uint8_t>();
    uint8_t destSlot = doc["toSlot"].as<uint8_t>();

    delegate->swapSnapshots(
        projectId, sourceBankNumber, sourceSlot, destBankNumber, destSlot);
}

void Api::setCurrentSnapshotBank(uint8_t bankNumber)
{
    delegate->setCurrentSnapshotBank(bankNumber);
}
