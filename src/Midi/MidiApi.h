#pragma once

#include "Setup/Setup.h"
#include "UiDelegate.h"

class MidiApi
{
public:
    explicit MidiApi(const MidiControls &newMidiControls,
                     UiDelegate &newDelegate)
        : midiControls(newMidiControls), delegate(newDelegate)
    {
    }

    virtual ~MidiApi() = default;

    bool process(const MidiMessage &midiMessage)
    {
        MidiMessage::Type type = midiMessage.getType();
        uint8_t data1 = midiMessage.getData1();
        uint8_t data2 = midiMessage.getData2();

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
        return (true);
    }

private:
    const MidiControls &midiControls;
    UiDelegate &delegate;
};
