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

struct PatchRequest {
    PatchRequest() : port(0), deviceId(0)
    {
    }

    PatchRequest(uint8_t port, uint8_t deviceId, std::vector<uint8_t> data)
        : port(port), deviceId(deviceId), data(data)
    {
    }

    uint8_t deviceId;
    uint8_t port;
    std::vector<uint8_t> data;
};

extern CircularBuffer<PatchRequest, 128> patchRequests;

class Midi
{
public:
    explicit Midi(const Preset &preset);
    virtual ~Midi() = default;

    void sendMessage(const Message &message);
    void sendTemplatedSysex(const Device &device, std::vector<uint8_t> data);
    void process(const MidiInput &midiInput, const MidiMessage &midiMessage);
    void requestAllPatches(void);

private:
    uint8_t transformMessage(const Device &deviceId,
                             std::vector<uint8_t> data,
                             uint8_t *dataOut);
    void runVariable(uint16_t &i,
                     uint16_t &j,
                     std::vector<uint8_t> data,
                     uint8_t *dataOut,
                     const Device &device);
    void runChecksum(uint16_t &i,
                     uint16_t &j,
                     std::vector<uint8_t> data,
                     uint8_t *dataOut,
                     const Device &device);
    void runLuaFunction(uint16_t &i,
                        uint16_t &j,
                        std::vector<uint8_t> data,
                        uint8_t *dataOut,
                        const Device &device);
    void runConstant(uint16_t &i,
                     uint16_t &j,
                     std::vector<uint8_t> data,
                     uint8_t *dataOut,
                     const Device &device);
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
    void processSysex(const MidiMessage &midiMessage);

    bool doesHeaderMatch(const SysexBlock &sysexBlock,
                         uint8_t header[],
                         uint8_t headerLength);
    void resetRulesValues(const Device &device, const Rules rules);
    void applyRulesValues(const Device &device,
                          const Rules rules,
                          const SysexBlock &sysexBlock,
                          uint16_t headerLength);

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
    static void
        sendSysEx(uint8_t port, uint8_t *data, uint16_t sysexDataLength);

    const Preset &model;

    RpnDetector rpnDetector;
    Cc14Detector cc14Detector;
};
