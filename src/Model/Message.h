#pragma once

#include "ElectraMessage.h"
#include "SignMode.h"
#include "Macros.h"
#include <stdint.h>
#include "helpers.h"

class Message
{
public:
    Message()
        : deviceId(0),
          parameterNumber(0),
          midiMin(0),
          midiMax(NOT_SET),
          value(NOT_SET),
          offValue(0),
          onValue(NOT_SET)
    {
        type = (uint8_t)ElectraMessageType::invalid;
        lsbFirst = false;
        signMode = (uint8_t)SignMode::noSign;
        bitWidth = 7;
    }

    Message(uint8_t newDeviceId,
            ElectraMessageType newType,
            uint16_t newParameterNumber,
            int16_t newMidiMin,
            int16_t newMidiMax,
            int16_t newValue,
            int16_t newOffValue,
            int16_t newOnValue,
            std::vector<uint8_t> newData,
            bool newLsbFirst,
            SignMode newSignMode,
            uint8_t newbitWidth)
        : deviceId(newDeviceId),
          parameterNumber(newParameterNumber),
          midiMin(newMidiMin),
          midiMax(newMidiMax),
          value(newValue),
          offValue(newOffValue),
          onValue(newOnValue),
          data(newData)
    {
        type = (uint8_t)newType;
        lsbFirst = newLsbFirst;
        signMode = (uint8_t)newSignMode;
        bitWidth = newbitWidth;

        if (newType == ElectraMessageType::note) {
            offValue = 0;
            onValue = 127; // 0 for note off and 127 for note on.
        }
    }

    void setDeviceId(uint8_t newDeviceId)
    {
        deviceId = newDeviceId;
    }

    uint8_t getDeviceId(void) const
    {
        return (deviceId);
    }

    void setType(ElectraMessageType newType)
    {
        type = (uint8_t)newType;
    }

    ElectraMessageType getType(void) const
    {
        return ((ElectraMessageType)type);
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
        offValue = newOffValue;
    }

    int16_t getOffValue(void) const
    {
        return (offValue);
    }

    void setOnValue(int16_t newOnValue)
    {
        onValue = newOnValue;
    }

    int16_t getOnValue(void) const
    {
        return (onValue);
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
	    logMessage("        deviceId: %d", getDeviceId());
	    logMessage("        type: %d", getType());
	    logMessage("        parameterNumber: %d", getParameterNumber());
	    logMessage("        min: %d", getMidiMin());
	    logMessage("        max: %d", getMidiMax());
	    logMessage("        value: %d", getValue());
	    logMessage("        onValue: %d", getOnValue());
	    logMessage("        offValue: %d", getOffValue());
	    logMessage("        signMode: %d", getSignMode());
	    logMessage("        lsbFirst: %d", getLsbFirst());
	    logMessage("        bitWidth: %d", getBitWidth());
	}

    std::vector<uint8_t> data;

private:
    uint8_t deviceId;
    uint16_t parameterNumber;
    int16_t midiMin;
    int16_t midiMax;
    int16_t value;
    int16_t offValue;
    int16_t onValue;

    struct {
        uint8_t type : 4;
        bool lsbFirst : 1;
        uint8_t signMode : 2;
        uint8_t bitWidth : 4;
    };
};
