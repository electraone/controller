#pragma once

#include <vector>
#include <String>
#include <functional>
#include "MidiBase.h"
#include "MidiOutput.h"
#include "Device.h"
#include "Message.h"
#include "Preset.h"
#include "Cc14Detector.h"
#include "RpnDetector.h"

class Midi
{
public:
    explicit Midi(const Preset &preset);
    virtual ~Midi() = default;

    void sendMessage(Message *message);
    void sendMessageNow(Message *message);
    void sendTemplatedSysex(Device *device, std::vector<uint8_t> data);
    // Lua functions. TODO: It does not really belong here.
    void registerLuaFunctions(std::vector<std::string> *newLuaFunctions);

    Device *getDevice(uint8_t port, uint8_t channel);
    Device *getDevice(uint8_t deviceId);

    void processMidi(const MidiInput &midiInput,
                     const MidiMessage &midiMessage);

private:
    uint8_t transformMessage(Device *deviceId,
                             std::vector<uint8_t> data,
                             uint8_t *dataOut);
    void processStart(void);
    void processStop(void);
    void processTuneRequest(void);
    void
        processCc(uint8_t deviceId, uint8_t midiParameterId, uint8_t midiValue);
    void processNote(uint8_t deviceId,
                     MidiMessage::Type midiType,
                     uint8_t noteNumber,
                     uint8_t velocity);
    void processProgramChange(uint8_t deviceId, uint8_t programNumber);

    std::vector<std::string> *luaFunctions;
    MidiOutput device1;
    RpnDetector rpnDetector;
    Cc14Detector cc14Detector;
    const Preset &model;
};
