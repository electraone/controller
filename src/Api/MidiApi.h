#pragma once

#include "Setup/Setup.h"
#include "MainDelegate.h"

class MidiApi
{
public:
    MidiApi(const MidiControls &newMidiControls, MainDelegate &newDelegate);
    virtual ~MidiApi() = default;

    void process(const MidiMessage &midiMessage);

private:
    void processBankSelect(uint8_t bankNumber);
    void processProgramChange(uint8_t programNumber);
    void processMidiControl(MidiMessage::Type type, uint8_t data1);

    enum ProgramChangeMode { Presets, Snapshots };

    const MidiControls &midiControls;
    MainDelegate &delegate;
    ProgramChangeMode programChangeMode;
};
