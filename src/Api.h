#pragma once

#include "UiDelegate.h"
#include "SysexBlock.h"

class Api
{
public:
    Api(UiDelegate *newDelegate) : delegate(newDelegate)
    {
    }

    virtual ~Api() = default;

    void sendSnapshotList(MemoryBlock &sysexPayload);
    void sendSnapshot(MemoryBlock &sysexPayload);
    void sendPresetList(void);
    void enableMidiLearn(void);
    void disableMidiLearn(void);
    void switchPreset(uint8_t bankNumber, uint8_t slot);
    void updateControl(uint16_t controlId, MemoryBlock &sysexPayload);
    void setSnapshotSlot(MemoryBlock &sysexPayload);
    void setPresetSlot(uint8_t bankNumber, uint8_t slot);
    void updateSnapshot(MemoryBlock &sysexPayload);
    void removeSnapshot(MemoryBlock &sysexPayload);
    void swapSnapshots(MemoryBlock &sysexPayload);
    void setCurrentSnapshotBank(uint8_t bankNumber);

private:
    UiDelegate *delegate;
};
