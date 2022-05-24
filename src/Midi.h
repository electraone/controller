#pragma once

#include <vector>
#include <String>
#include <functional>
#include "MidiOutput.h"
#include "Device.h"
#include "Message.h"
#include "Preset.h"
#include "Cc14Detector.h"
#include "RpnDetector.h"
#include "ElectraMessage.h"

class Midi
{
public:
    explicit Midi(const Preset &preset);
    virtual ~Midi() = default;

    void sendMessage(const Message &message);
    void sendTemplatedSysex(const Device &device, std::vector<uint8_t> data);
    // Lua functions. TODO: It does not really belong here.
    void registerLuaFunctions(std::vector<std::string> *newLuaFunctions);

    Device *getDevice(uint8_t port, uint8_t channel);
    Device *getDevice(uint8_t deviceId);

    void processMidi(const MidiInput &midiInput,
                     const MidiMessage &midiMessage);

private:
    uint8_t transformMessage(const Device &deviceId,
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

    static void sendControlChange(uint8_t port,
                                  uint8_t channel,
                                  uint8_t parameterNumber,
                                  uint8_t value);
    static void sendControlChange14Bit(uint8_t port,
                                       uint8_t channel,
                                       uint16_t parameterNumber,
                                       uint16_t midiValue,
                                       bool lsbFirst);
    static void sendNrpn(uint8_t port,
                         uint8_t channel,
                         uint16_t parameterNumber,
                         uint16_t midiValue,
                         bool lsbFirst);
    static void sendRpn(uint8_t port,
                        uint8_t channel,
                        uint16_t parameterNumber,
                        uint16_t midiValue);
    static void
        sendProgramChange(uint8_t port, uint8_t channel, uint8_t programNumber);

    static void sendNoteOn(uint8_t port,
                           uint8_t channel,
                           uint8_t noteNumber,
                           uint8_t velocity);
    static void sendNoteOff(uint8_t port,
                            uint8_t channel,
                            uint8_t noteNumber,
                            uint8_t velocity);

    static void sendStart(uint8_t port);
    static void sendStop(uint8_t port);
    static void sendTuneRequest(uint8_t port);

    std::vector<std::string> *luaFunctions;

    RpnDetector rpnDetector;
    Cc14Detector cc14Detector;
    const Preset &model;
};
