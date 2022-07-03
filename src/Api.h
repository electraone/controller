#pragma once

#include "UiDelegate.h"
#include "SysexBlock.h"

class Api
{
public:
    explicit Api(UiDelegate &newDelegate);
    virtual ~Api() = default;

    void sendSnapshotList(uint8_t port, MemoryBlock &sysexPayload);
    void sendSnapshot(uint8_t port, MemoryBlock &sysexPayload);
    void sendPresetList(uint8_t port);
    void enableMidiLearn(void);
    void disableMidiLearn(void);
    void switchPreset(uint8_t bankNumber, uint8_t slot);
    void updateControl(uint16_t controlId, MemoryBlock &sysexPayload);
    void setSnapshotSlot(MemoryBlock &sysexPayload);
    void importSnapshot(LocalFile file);
    void setPresetSlot(uint8_t bankNumber, uint8_t slot);
    void updateSnapshot(MemoryBlock &sysexPayload);
    void removeSnapshot(MemoryBlock &sysexPayload);
    void swapSnapshots(MemoryBlock &sysexPayload);
    void setCurrentSnapshotBank(uint8_t bankNumber);

private:
    UiDelegate &delegate;
};
