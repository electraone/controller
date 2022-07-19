#include "Midi.h"
#include "Event.h"
#include "Checksum.h"
#include "luaHooks.h"
#include "luaPatch.h"
#include "ParameterMap.h"

CircularBuffer<PatchRequest, 128> patchRequests;

/** Constructor
 *
 */
Midi::Midi(const Preset &preset) : model(preset)
{
}

/** Send Electra message to Midi outputs.
 *
 */
void Midi::sendMessage(const Message &message)
{
    Device device = model.getDevice(message.getDeviceId());

    if (!device.isValid()) {
        return;
    }

    uint8_t channel = device.getChannel();
    uint8_t port = device.getPort();
    uint16_t midiValue = message.getValue();
    Event event = message.getEvent();

    // Set the timestamp of device last message to current time
    device.setTsLastMessage();

    // Send the particular MIDI message
    if (message.getType() == ElectraMessageType::cc7) {
        sendControlChange(
            port, channel, message.getParameterNumber(), midiValue);
    } else if (message.getType() == ElectraMessageType::cc14) {
        sendControlChange14Bit(port,
                               channel,
                               message.getParameterNumber(),
                               midiValue,
                               message.getLsbFirst());
    } else if (message.getType() == ElectraMessageType::nrpn) {
        sendNrpn(port,
                 channel,
                 message.getParameterNumber(),
                 midiValue,
                 message.getLsbFirst());
    } else if (message.getType() == ElectraMessageType::rpn) {
        sendRpn(port, channel, message.getParameterNumber(), midiValue);
    } else if (message.getType() == ElectraMessageType::program) {
        sendProgramChange(port, channel, midiValue);
    } else if (message.getType() == ElectraMessageType::note) {
        if (event == Event::press) {
            sendNoteOff(port,
                        channel,
                        message.getParameterNumber(),
                        127); //midiValue);
        } else {
            sendNoteOff(port,
                        channel,
                        message.getParameterNumber(),
                        127); //midiValue);
        }
    } else if (message.getType() == ElectraMessageType::start) {
        if (event == Event::press) {
            sendStart(port);
        }
    } else if (message.getType() == ElectraMessageType::stop) {
        if (event == Event::press) {
            sendStop(port);
        }
    } else if (message.getType() == ElectraMessageType::tune) {
        if (event == Event::press) {
            sendTuneRequest(port);
        }
    } else if (message.getType() == ElectraMessageType::sysex) {
        sendTemplatedSysex(device, message.data);
    }
}

/** Send a SysEx message with the placeholder substituted
 *
 */
void Midi::sendTemplatedSysex(const Device &device, std::vector<uint8_t> data)
{
    const int maxSysexSize = 512;

    if (data.size() < maxSysexSize) {
        uint8_t sysexData[maxSysexSize];
        uint16_t sysexDataLength = 0;
        sysexDataLength = transformMessage(device, data, sysexData);
        sendSysEx(device.getPort(), sysexData, sysexDataLength);
    } else {
        logMessage(
            "sendTemplatedSysex: message exceeds allowed maximum length: %d",
            maxSysexSize);
    }
}

/** Replace substitution variables with values
 *
 */
uint8_t Midi::transformMessage(const Device &device,
                               std::vector<uint8_t> data,
                               uint8_t *dataOut)
{
    uint16_t j = 0;

    for (uint16_t i = 0; i < data.size(); i++) {
        if (data[i] == VARIABLE_DATA) {
            runVariable(i, j, data, dataOut, device);
        } else if (data[i] == CHECKSUM) {
            runChecksum(i, j, data, dataOut, device);
        } else if (data[i] == LUAFUNCTION) {
            runLuaFunction(i, j, data, dataOut, device);
        } else {
            runConstant(i, j, data, dataOut, device);
        }
    }
    return (j);
}

void Midi::runVariable(uint16_t &i,
                       uint16_t &j,
                       std::vector<uint8_t> data,
                       uint8_t *dataOut,
                       const Device &device)
{
    uint8_t type = 0;
    uint8_t parameterIdLSB;
    uint8_t parameterIdMSB;
    uint16_t parameterId = 0;
    uint8_t pPos = 0;
    uint8_t bPos = 0;
    uint8_t size = 0;
    uint8_t byteToSend = 0;
    uint16_t parameterValue = 0;
    uint16_t mask = 0;

    i++;

    while ((data[i] != VARIABLE_DATA_END) && (i < 100)) {
        type = data[i];
        i++;
        parameterIdLSB = data[i];
        i++;
        parameterIdMSB = data[i];
        i++;
        parameterId = parameterIdLSB + (parameterIdMSB * 128);
        pPos = data[i];
        i++;
        bPos = data[i];
        i++;
        size = data[i];
        i++;
        mask = createMask(pPos, size);

        parameterValue =
            ((parameterMap.getValue(
                  device.getId(), (ElectraMessageType)type, parameterId)
              & mask)
             >> pPos)
            << bPos;
        byteToSend |= parameterValue;
    }

    dataOut[j] = byteToSend & 0x7F; // mask to 7bit and assign to output array
    j++;
}

void Midi::runChecksum(uint16_t &i,
                       uint16_t &j,
                       std::vector<uint8_t> data,
                       uint8_t *dataOut,
                       const Device &device)
{
    i++;
    ChecksumAlgorithm algorithm = (ChecksumAlgorithm)data[i];
    i++;
    uint8_t start = data[i];
    i++;
    uint8_t length = data[i];
    uint8_t checksum = 0;

    if (algorithm == ChecksumAlgorithm::ROLAND) {
        checksum = calculateChecksum(&dataOut[start], length);
    } else if (algorithm == ChecksumAlgorithm::FRACTALAUDIO) {
        checksum = calculateChecksum_fractalaudio(&dataOut[start], length);
    } else if (algorithm == ChecksumAlgorithm::WALDORF) {
        checksum = calculateChecksum_waldorf(&dataOut[start], length);
    } else {
        checksum = 0;
    }

    dataOut[j] = checksum & 0x7F;
    j++;

    logMessage(
        "Checksum calculation: algorithm=%d, start=%d, length=%d, checksum=%d",
        algorithm,
        start,
        length,
        checksum);
}

void Midi::runLuaFunction(uint16_t &i,
                          uint16_t &j,
                          std::vector<uint8_t> data,
                          uint8_t *dataOut,
                          const Device &device)
{
    uint8_t type = 0;
    uint8_t parameterIdLSB;
    uint8_t parameterIdMSB;
    uint16_t parameterId = 0;
    uint16_t parameterValue = 0;
    uint8_t byteToSend = 0;

    i++;
    type = data[i];
    i++;
    parameterIdLSB = data[i];
    i++;
    parameterIdMSB = data[i];
    i++;
    uint8_t functionId = data[i];

    logMessage(
        "function: %d (%s)", functionId, luaFunctions[functionId].c_str());

    parameterId = parameterIdLSB + (parameterIdMSB * 128);
    parameterValue = parameterMap.getValue(
        device.getId(), (ElectraMessageType)type, parameterId);

    if (L && (luaFunctions.size() > 0)) {
        byteToSend = runTemplateFunction(
            luaFunctions[functionId].c_str(), (void *)&device, parameterValue);
    }

    dataOut[j] = byteToSend & 0x7F;
    j++;
}

void Midi::runConstant(uint16_t &i,
                       uint16_t &j,
                       std::vector<uint8_t> data,
                       uint8_t *dataOut,
                       const Device &device)
{
    dataOut[j] = data[i];
    j++;
}

/** Process incoming MIDI messages (when MIDI learn is off)
 *
 */
void Midi::process(const MidiInput &midiInput, const MidiMessage &midiMessage)
{
    Device device =
        model.getDevice(midiInput.getPort(), midiMessage.getChannel());

    // Non-Channel messages (TODO: it ignores ports)
    if (!device.isValid()) {
        if (midiMessage.isMidiStart()) {
            processStart();
        } else if (midiMessage.isMidiStop()) {
            processStop();
        } else if (midiMessage.isMidiTuneRequest()) {
            processTuneRequest();
        } else if (midiMessage.isSysEx()) {
            processSysex(midiMessage);
        }
    }

    // Channel Messages
    else {
        uint8_t deviceId = device.getId();

        if (midiMessage.isController()) {
            processCc(deviceId, midiMessage.getData1(), midiMessage.getData2());
        } else if (midiMessage.isNote()) {
            processNote(deviceId,
                        midiMessage.getType(),
                        midiMessage.getData1(),
                        midiMessage.getData2());
        } else if (midiMessage.isProgramChange()) {
            processProgramChange(deviceId, midiMessage.getData1());
        } else {
            // logMessage("Midi::processMidi: other midi message. ignoring it.");
        }
    }
}

void Midi::processStart(void)
{
    logMessage("Midi::processMidi: Start midi message");
    parameterMap.setValue(0xff, ElectraMessageType::start, 0, 0, Origin::midi);
}

void Midi::processStop(void)
{
    logMessage("Midi::processMidi: Stop midi message");
    parameterMap.setValue(0xff, ElectraMessageType::stop, 0, 0, Origin::midi);
}

void Midi::processTuneRequest(void)
{
    logMessage("Midi::processMidi: Tune midi message");
    parameterMap.setValue(0xff, ElectraMessageType::tune, 0, 0, Origin::midi);
}

void Midi::processCc(uint8_t deviceId,
                     uint8_t midiParameterId,
                     uint8_t midiValue)
{
    logMessage(
        "ElectraMidi: processMidi: Control change midi message: parameter=%d, value=%d",
        midiParameterId,
        midiValue);

    // handle RPN / NRPN
    MidiRpnMessage midiRpnMessage;

    if (rpnDetector.parseControllerMessage(
            deviceId, midiParameterId, midiValue, midiRpnMessage)) {
        logMessage("Midi::processMidi: RPN detected: parameter=%d, "
                   "value=%d, isNrpn=%d, is14bit=%d",
                   midiRpnMessage.parameterNumber,
                   midiRpnMessage.value,
                   midiRpnMessage.isNrpn,
                   midiRpnMessage.is14BitValue);

        ElectraMessageType electraMessageType = (midiRpnMessage.isNrpn)
                                                    ? ElectraMessageType::nrpn
                                                    : ElectraMessageType::rpn;

        parameterMap.setValue(deviceId,
                              electraMessageType,
                              midiRpnMessage.parameterNumber,
                              midiRpnMessage.value,
                              Origin::midi);
    }

    // handle CC14
    MidiCc14Message midiCc14Message;

    if (cc14Detector.parseControllerMessage(
            deviceId, midiParameterId, midiValue, midiCc14Message)) {
        logMessage("Midi::processMidi: CC14 detected: parameter=%d, value=%d",
                   midiCc14Message.parameterNumber,
                   midiCc14Message.value);
        parameterMap.setValue(deviceId,
                              ElectraMessageType::cc14,
                              midiCc14Message.parameterNumber,
                              midiCc14Message.value,
                              Origin::midi);
    }

    // Save to the parameter map
    parameterMap.setValue(deviceId,
                          ElectraMessageType::cc7,
                          midiParameterId,
                          midiValue,
                          Origin::midi);
}

void Midi::processNote(uint8_t deviceId,
                       MidiMessage::Type midiType,
                       uint8_t noteNumber,
                       uint8_t velocity)
{
    uint8_t translatedVelocity =
        (midiType == MidiMessage::Type::NoteOn) ? velocity : 0;
    logMessage("Midi::processMidi: note message: note=%d, velocity=%d",
               noteNumber,
               translatedVelocity);
    parameterMap.setValue(deviceId,
                          ElectraMessageType::note,
                          noteNumber,
                          translatedVelocity,
                          Origin::midi);
}

void Midi::processProgramChange(uint8_t deviceId, uint8_t programNumber)
{
    logMessage("Midi::processMidi: program message: program=%d", programNumber);
    parameterMap.setValue(
        deviceId, ElectraMessageType::program, 0, programNumber, Origin::midi);
}

void Midi::processSysex(const MidiMessage &midiMessage)
{
    SysexBlock sysexBlock = midiMessage.getSysExBlock();

    if (sysexBlock.isEmpty()) {
        return;
    }

    for (const auto &[id, device] : model.devices) {
        for (const auto &response : device.responses) {
            // \todo this can easily overflow the buffer
            uint8_t header[64];
            uint8_t headerLength = 0;
            headerLength = transformMessage(device, response.headers, header);

            if (doesHeaderMatch(sysexBlock, header, headerLength) == true) {
                logMessage(
                    "Midi::processSysex: matched response: responseId=%d",
                    response.getId());

                // Run Lua onResponse function
                if (L) {
                    runOnResponse(device, response.getId(), sysexBlock);
                }

                resetRulesValues(device, response.rules);
                applyRulesValues(
                    device, response.rules, sysexBlock, headerLength);
                break;
            }
        }
    }
}

bool Midi::doesHeaderMatch(const SysexBlock &sysexBlock,
                           uint8_t header[],
                           uint8_t headerLength)
{
    bool match = true;
    auto sysexLength = sysexBlock.getLength();

    if ((sysexLength - 2) >= headerLength) {
        for (uint8_t i = 1; i < headerLength + 1; i++) {
            if (header[i - 1] != sysexBlock.peek(i)) {
                match = false;
                break;
            }
        }
    } else {
        match = false;
    }

    return (match);
}

void Midi::resetRulesValues(const Device &device, const Rules rules)
{
    for (const auto &rule : rules) {
        parameterMap.setValue(device.getId(),
                              rule.getType(),
                              rule.getParameterNumber(),
                              0,
                              Origin::midi);
    }
}

void Midi::applyRulesValues(const Device &device,
                            const Rules rules,
                            const SysexBlock &sysexBlock,
                            uint16_t headerLength)
{
    for (const auto &rule : rules) {
        uint16_t bytePosition = rule.getByte() + headerLength + 1;
        int sysexByte = sysexBlock.peek(bytePosition);

        // If the sysexBlock byte exists
        if (sysexByte != -1) {
            uint16_t mask =
                createMask(rule.getByteBitPosition(), rule.getBitWidth());
            uint16_t parameterValue = (((sysexByte & mask) >> getShift(mask))
                                       << rule.getParameterBitPosition());
            LookupEntry *entry =
                parameterMap.applyToValue(device.getId(),
                                          rule.getType(),
                                          rule.getParameterNumber(),
                                          parameterValue,
                                          Origin::midi);

            if (entry) {
                logMessage(
                    "Midi::applyRulesValues: applying extraction rule: byte=%d, "
                    "byteValue=%d, extractedValue=%d to parameterNumber=%d, type=%s "
                    "resulting in parameterValue=%d",
                    rule.getByte(),
                    sysexByte,
                    parameterValue,
                    rule.getParameterNumber(),
                    translateElectraMessageTypeToText(rule.getType()),
                    entry->midiValue);
            }
        }
    }
}

void Midi::requestAllPatches(void)
{
    for (auto &[id, device] : model.devices) {
        for (const auto &request : device.requests) {
            patchRequests.push(
                PatchRequest(device.getPort(), device.getId(), request));
        }

        if (L) {
            runOnRequest(device);
        }
    }

    if (!patchRequests.isEmpty()) {
        System::tasks.enableUserTask();
    }
}

void Midi::sendControlChange(uint8_t port,
                             uint8_t channel,
                             uint8_t parameterNumber,
                             uint8_t value)
{
    MidiOutput::sendControlChange(
        MidiInterface::Type::MidiAll, port, channel, parameterNumber, value);
}

void Midi::sendControlChange14Bit(uint8_t port,
                                  uint8_t channel,
                                  uint16_t parameterNumber,
                                  uint16_t midiValue,
                                  bool lsbFirst)
{
    MidiOutput::sendControlChange14Bit(MidiInterface::Type::MidiAll,
                                       port,
                                       channel,
                                       parameterNumber,
                                       midiValue,
                                       lsbFirst);
}

void Midi::sendNrpn(uint8_t port,
                    uint8_t channel,
                    uint16_t parameterNumber,
                    uint16_t midiValue,
                    bool lsbFirst)
{
    MidiOutput::sendNrpn(MidiInterface::Type::MidiAll,
                         port,
                         channel,
                         parameterNumber,
                         midiValue,
                         lsbFirst);
}

void Midi::sendRpn(uint8_t port,
                   uint8_t channel,
                   uint16_t parameterNumber,
                   uint16_t midiValue)
{
    MidiOutput::sendRpn(MidiInterface::Type::MidiAll,
                        port,
                        channel,
                        parameterNumber,
                        midiValue);
}

void Midi::sendProgramChange(uint8_t port,
                             uint8_t channel,
                             uint8_t programNumber)
{
    MidiOutput::sendProgramChange(
        MidiInterface::Type::MidiAll, port, channel, programNumber);
}

void Midi::sendNoteOn(uint8_t port,
                      uint8_t channel,
                      uint8_t noteNumber,
                      uint8_t velocity)
{
    MidiOutput::sendNoteOn(
        MidiInterface::Type::MidiAll, port, channel, noteNumber, velocity);
}

void Midi::sendNoteOff(uint8_t port,
                       uint8_t channel,
                       uint8_t noteNumber,
                       uint8_t velocity)
{
    MidiOutput::sendNoteOff(
        MidiInterface::Type::MidiAll, port, channel, noteNumber, velocity);
}

void Midi::sendStart(uint8_t port)
{
    MidiOutput::sendStart(MidiInterface::Type::MidiAll, port);
}

void Midi::sendStop(uint8_t port)
{
    MidiOutput::sendStop(MidiInterface::Type::MidiAll, port);
}

void Midi::sendTuneRequest(uint8_t port)
{
    MidiOutput::sendTuneRequest(MidiInterface::Type::MidiAll, port);
}

void Midi::sendSysEx(uint8_t port, uint8_t *sysexData, uint16_t sysexDataLength)
{
    MidiOutput::sendSysEx(
        MidiInterface::Type::MidiAll, port, sysexData, sysexDataLength);
}
