#include "Midi.h"
#include "Event.h"
#include "Checksum.h"
#include "luaHooks.h"
#include "ParameterMap.h"

/** Constructor
 *
 */
Midi::Midi(const Preset &preset) : luaFunctions(nullptr), model(preset)
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
    static const int maxSysexSize = 512;

    if (data.size() < maxSysexSize) {
        uint8_t sysexData[maxSysexSize];
        uint8_t sysexDataLength = 0;

        sysexDataLength = transformMessage(device, data, sysexData);

        MidiOutput::sendSysEx(MidiInterface::Type::MidiUsbDev,
                              device.getPort(),
                              sysexData,
                              sysexDataLength);
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
    uint8_t j = 0;

    for (uint8_t i = 0; i < data.size(); i++) {
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

        if (data[i] == VARIABLE_DATA) {
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

            dataOut[j] =
                byteToSend & 0x7F; // mask to 7bit and assign to output array
            j++;
        } else if (data[i] == CHECKSUM) {
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
                checksum =
                    calculateChecksum_fractalaudio(&dataOut[start], length);
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
        } else if (data[i] == LUAFUNCTION) {
            i++;
            type = data[i];
            i++;
            parameterIdLSB = data[i];
            i++;
            parameterIdMSB = data[i];
            i++;
            uint8_t functionId = data[i];

            parameterId = parameterIdLSB + (parameterIdMSB * 128);
            parameterValue = parameterMap.getValue(
                device.getId(), (ElectraMessageType)type, parameterId);

            if (L && luaFunctions) {
                byteToSend =
                    runTemplateFunction((*luaFunctions)[functionId].c_str(),
                                        (void *)&device,
                                        parameterValue);
            }

            dataOut[j] = byteToSend & 0x7F; // output of function
            j++;
        } else {
            dataOut[j] = data[i];
            j++;
        }
    }
    return (j);
}

/** Register all available Lua functions
 *
 */
void Midi::registerLuaFunctions(std::vector<std::string> *newLuaFunctions)
{
    luaFunctions = newLuaFunctions;
}

/** Process incoming MIDI messages (when MIDI learn is off)
 *
 */
void Midi::processMidi(const MidiInput &midiInput,
                       const MidiMessage &midiMessage)
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
            logMessage("Midi::processMidi: other midi message. ignoring it.");
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

void Midi::sendControlChange(uint8_t port,
                             uint8_t channel,
                             uint8_t parameterNumber,
                             uint8_t value)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendControlChange(
            interfaceType, port, channel, parameterNumber, value);
    }
}

void Midi::sendControlChange14Bit(uint8_t port,
                                  uint8_t channel,
                                  uint16_t parameterNumber,
                                  uint16_t midiValue,
                                  bool lsbFirst)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendControlChange14Bit(
            interfaceType, port, channel, parameterNumber, midiValue, lsbFirst);
    }
}

void Midi::sendNrpn(uint8_t port,
                    uint8_t channel,
                    uint16_t parameterNumber,
                    uint16_t midiValue,
                    bool lsbFirst)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendNrpn(
            interfaceType, port, channel, parameterNumber, midiValue, lsbFirst);
    }
}

void Midi::sendRpn(uint8_t port,
                   uint8_t channel,
                   uint16_t parameterNumber,
                   uint16_t midiValue)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendRpn(
            interfaceType, port, channel, parameterNumber, midiValue);
    }
}

void Midi::sendProgramChange(uint8_t port,
                             uint8_t channel,
                             uint8_t programNumber)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendProgramChange(
            interfaceType, port, channel, programNumber);
    }
}

void Midi::sendNoteOn(uint8_t port,
                      uint8_t channel,
                      uint8_t noteNumber,
                      uint8_t velocity)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendNoteOn(
            interfaceType, port, channel, noteNumber, velocity);
    }
}

void Midi::sendNoteOff(uint8_t port,
                       uint8_t channel,
                       uint8_t noteNumber,
                       uint8_t velocity)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendNoteOff(
            interfaceType, port, channel, noteNumber, velocity);
    }
}

void Midi::sendStart(uint8_t port)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendStart(interfaceType, port);
    }
}

void Midi::sendStop(uint8_t port)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendStop(interfaceType, port);
    }
}

void Midi::sendTuneRequest(uint8_t port)
{
    for (const auto &interfaceType : MidiInterface::allInterfaceTypes) {
        MidiOutput::sendTuneRequest(interfaceType, port);
    }
}
