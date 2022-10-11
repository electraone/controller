#pragma once

#include "SignMode.h"
#include "Macros.h"
#include "Event.h"
#include <cstdint>
#include <cstring>
#include "helpers.h"

#define MIDI_VALUE_DO_NOT_SEND 16537

class Message
{
public:
    enum Type {
        virt = 0,
        cc7 = 1,
        cc14 = 2,
        nrpn = 3,
        rpn = 4,
        note = 5,
        program = 6,
        sysex = 7,
        start = 8,
        stop = 9,
        tune = 10,
        atpoly = 11,
        atchannel = 12,
        pitchbend = 13,
        spp = 14,
        invalid = 15
    };

    Message()
        : deviceId(0),
          parameterNumber(0),
          midiMin(0),
          midiMax(NOT_SET),
          value(NOT_SET)
    {
        type = (uint8_t)Type::invalid;
        lsbFirst = false;
        signMode = (uint8_t)SignMode::noSign;
        bitWidth = 7;
    }

    Message(uint8_t newDeviceId,
            Type newType,
            uint16_t newParameterNumber,
            int16_t newMidiMin,
            int16_t newMidiMax,
            int16_t newValue,
            std::vector<uint8_t> newData,
            bool newLsbFirst,
            SignMode newSignMode,
            uint8_t newbitWidth)
        : deviceId(newDeviceId),
          parameterNumber(newParameterNumber),
          midiMin(newMidiMin),
          midiMax(newMidiMax),
          value(newValue),
          data(newData)
    {
        type = (uint8_t)newType;
        lsbFirst = newLsbFirst;
        signMode = (uint8_t)newSignMode;
        bitWidth = newbitWidth;
    }

    virtual ~Message() = default;

    void setDeviceId(uint8_t newDeviceId)
    {
        deviceId = newDeviceId;
    }

    uint8_t getDeviceId(void) const
    {
        return (deviceId);
    }

    void setType(Type newType)
    {
        type = (uint8_t)newType;
    }

    Type getType(void) const
    {
        return ((Type)type);
    }

    void setMidiMin(int16_t newMin)
    {
        midiMin = newMin;
    }

    int16_t getMidiMin(void) const
    {
        return (midiMin);
    }

    int16_t getMidiMax(void) const
    {
        return (midiMax);
    }

    void setMidiMax(int16_t newMax)
    {
        midiMax = newMax;
    }

    void setParameterNumber(uint16_t newParameterNumber)
    {
        parameterNumber = newParameterNumber;
    }

    uint16_t getParameterNumber(void) const
    {
        return (parameterNumber);
    }

    void setValue(int16_t newValue)
    {
        value = newValue;
    }

    int16_t getValue(void) const
    {
        return (value);
    }

    void setSignMode(SignMode newSignMode)
    {
        signMode = (uint8_t)newSignMode;
    }

    SignMode getSignMode(void) const
    {
        return ((SignMode)signMode);
    }

    void setOffValue(int16_t newOffValue)
    {
        midiMin = newOffValue;
    }

    int16_t getOffValue(void) const
    {
        return (midiMin);
    }

    void setOnValue(int16_t newOnValue)
    {
        midiMax = newOnValue;
    }

    int16_t getOnValue(void) const
    {
        return (midiMax);
    }

    void setLsbFirst(bool newLsbFirst)
    {
        lsbFirst = newLsbFirst;
    }

    bool getLsbFirst(void) const
    {
        return (lsbFirst);
    }

    void setBitWidth(uint8_t newBitWidth)
    {
        bitWidth = newBitWidth;
    }

    uint8_t getBitWidth(void) const
    {
        return (bitWidth);
    }

    void print(void) const
    {
        logMessage("--");
        logMessage("        deviceId: %d", getDeviceId());
        logMessage(
            "        type: %s (%d)", translateType(getType()), getType());
        logMessage("        parameterNumber: %d", getParameterNumber());
        logMessage("        min: %d", getMidiMin());
        logMessage("        max: %d", getMidiMax());
        logMessage("        value: %d", getValue());
        logMessage("        signMode: %d", getSignMode());
        logMessage("        lsbFirst: %d", getLsbFirst());
        logMessage("        bitWidth: %d", getBitWidth());
    }

    static Type translateType(const char *typeText)
    {
        if (typeText) {
            if (strcmp(typeText, "cc7") == 0) {
                return (Type::cc7);
            } else if (strcmp(typeText, "cc14") == 0) {
                return (Type::cc14);
            } else if (strcmp(typeText, "nrpn") == 0) {
                return (Type::nrpn);
            } else if (strcmp(typeText, "rpn") == 0) {
                return (Type::rpn);
            } else if (strcmp(typeText, "note") == 0) {
                return (Type::note);
            } else if (strcmp(typeText, "program") == 0) {
                return (Type::program);
            } else if (strcmp(typeText, "sysex") == 0) {
                return (Type::sysex);
            } else if (strcmp(typeText, "start") == 0) {
                return (Type::start);
            } else if (strcmp(typeText, "stop") == 0) {
                return (Type::stop);
            } else if (strcmp(typeText, "tune") == 0) {
                return (Type::tune);
            } else if (strcmp(typeText, "virtual") == 0) {
                return (Type::virt);
            } else if (strcmp(typeText, "atpoly") == 0) {
                return (Type::atpoly);
            } else if (strcmp(typeText, "atchannel") == 0) {
                return (Type::atchannel);
            } else if (strcmp(typeText, "pitchbend") == 0) {
                return (Type::pitchbend);
            } else if (strcmp(typeText, "spp") == 0) {
                return (Type::spp);
            }
        } else {
            // default - for rules that do not have type specified
            return (Type::sysex);
        }
        return (Type::invalid);
    }

    static const char *translateType(Type messageType)
    {
        if (messageType == Type::cc7) {
            return ("cc7");
        } else if (messageType == Type::cc14) {
            return ("cc14");
        } else if (messageType == Type::nrpn) {
            return ("nrpn");
        } else if (messageType == Type::rpn) {
            return ("rpn");
        } else if (messageType == Type::note) {
            return ("note");
        } else if (messageType == Type::program) {
            return ("program");
        } else if (messageType == Type::sysex) {
            return ("sysex");
        } else if (messageType == Type::start) {
            return ("start");
        } else if (messageType == Type::stop) {
            return ("stop");
        } else if (messageType == Type::tune) {
            return ("tune");
        } else if (messageType == Type::virt) {
            return ("virtual");
        } else if (messageType == Type::atpoly) {
            return ("atpoly");
        } else if (messageType == Type::atchannel) {
            return ("atchannel");
        } else if (messageType == Type::pitchbend) {
            return ("pitchbend");
        } else if (messageType == Type::spp) {
            return ("spp");
        }

        return ("invalid");
    }

    std::vector<uint8_t> data;

private:
    uint16_t parameterNumber;
    int16_t value;
    int16_t midiMin;
    int16_t midiMax;

    struct {
        uint8_t deviceId : 5;
        uint8_t type : 4;
        bool lsbFirst : 1;
        uint8_t signMode : 2;
        uint8_t bitWidth : 4;
    };
};
