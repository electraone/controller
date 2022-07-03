#pragma once

#include "Setup/Setup.h"
#include "UiDelegate.h"

class MidiApi
{
public:
    explicit MidiApi(const MidiControls &newMidiControls,
                     UiDelegate &newDelegate)
        : midiControls(newMidiControls),
          delegate(newDelegate),
          programChangeMode(Presets)
    {
    }

    virtual ~MidiApi() = default;

    bool process(const MidiMessage &midiMessage)
    {
        if (midiMessage.isBankSelect()) {
            (void)processBankSelect(midiMessage.getData2());
        } else if (midiMessage.isProgramChange()) {
            (void)processProgramChange(midiMessage.getData1());
        }

        processMidiControl(midiMessage.getType(), midiMessage.getData1());

        return (true);
    }

private:
    bool processBankSelect(uint8_t bankNumber)
    {
        // Bank numbers 1 .. 12 are for the snapshot banks
        if ((0 < bankNumber) && (bankNumber <= 12)) {
            logMessage("processBankSelect: switching to the snapshot mode");
            programChangeMode = Snapshots;
            delegate.setCurrentSnapshotBank(bankNumber - 1);
            return (true);
        } else if (bankNumber == 0) {
            logMessage("processBankSelect: switching to the preset mode");
            programChangeMode = Presets;
            return (true);
        }

        return (false);
    }

    bool processProgramChange(uint8_t programNumber)
    {
        if (programChangeMode == Presets) {
            if (programNumber < 72) {
                uint8_t bankNumber = programNumber / 12;
                uint8_t slot = programNumber % 12;
                delegate.switchPreset(bankNumber, slot);
                return (true);
            }
        } else {
            if ((0 < programNumber) && (programNumber <= 36)) {
                uint8_t bankNumber = delegate.getCurrentSnapshotBank();
                const char *projectId = delegate.getCurrentProjectId();
                delegate.loadSnapshot(projectId, bankNumber, programNumber - 1);
            }
            return (true);
        }

        return (false);
    }

    void processMidiControl(MidiMessage::Type type, uint8_t data1)
    {
        for (const auto &midiControl : midiControls) {
            if (midiControl.midiMessageType == type) {
                if (midiControl.parameterNumber == data1) {
                    delegate.closeAllWindows();
                    uint8_t presetId = 0;
                    switch (midiControl.eventType) {
                        case AppEventType::switchPreset:
                            presetId = midiControl.eventParameter1
                                       - 1; // zero based index is needed
                            delegate.switchPreset(presetId / 12, presetId % 12);
                            break;

                        case AppEventType::switchPage:
                            delegate.switchPage(midiControl.eventParameter1);
                            break;

                        case AppEventType::switchPageNext:
                            delegate.switchPageNext();
                            break;

                        case AppEventType::switchPagePrev:
                            delegate.switchPagePrev();
                            break;

                        case AppEventType::switchPresetNext:
                            delegate.switchPresetNext();
                            break;

                        case AppEventType::switchPresetPrev:
                            delegate.switchPresetPrev();
                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }

    enum ProgramChangeMode { Presets, Snapshots };

    const MidiControls &midiControls;
    UiDelegate &delegate;
    ProgramChangeMode programChangeMode;
};
