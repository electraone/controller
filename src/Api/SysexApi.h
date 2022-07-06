#pragma once

#include "MainDelegate.h"
#include "SysexBlock.h"

class SysexApi
{
public:
    explicit SysexApi(MainDelegate &newDelegate);
    virtual ~SysexApi() = default;

    void process(uint8_t port, const SysexBlock &sysexBlock);
    void
        process(uint8_t port, LocalFile &file, ElectraCommand::Object fileType);

private:
    bool loadPreset(uint8_t port, LocalFile &file);
    bool loadLua(uint8_t port, LocalFile &file);
    bool loadConfig(uint8_t port, LocalFile &file);
    bool importSnapshot(uint8_t port, LocalFile &file);

    void sendSnapshotList(uint8_t port, MemoryBlock &sysexPayload);
    void sendSnapshot(uint8_t port, MemoryBlock &sysexPayload);
    void sendPresetList(uint8_t port);
    void enableMidiLearn(uint8_t port);
    void disableMidiLearn(uint8_t port);
    void switchPreset(uint8_t port, uint8_t bankNumber, uint8_t slot);
    void updateControl(uint8_t port,
                       uint16_t controlId,
                       MemoryBlock &sysexPayload);
    void setSnapshotSlot(uint8_t port, MemoryBlock &sysexPayload);
    void setPresetSlot(uint8_t port, uint8_t bankNumber, uint8_t slot);
    void updateSnapshot(uint8_t port, MemoryBlock &sysexPayload);
    void removeSnapshot(uint8_t port, MemoryBlock &sysexPayload);
    void swapSnapshots(uint8_t port, MemoryBlock &sysexPayload);
    void setCurrentSnapshotBank(uint8_t port, uint8_t bankNumber);

    MainDelegate &delegate;
};
