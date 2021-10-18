#pragma once

#include "ElectraMessage.h"
#include "SignMode.h"
#include "Macros.h"
#include "helpers.h"
#include "Event.h"
#include <stdint.h>
#include <vector>

class Message
{
	public:
		Message () :
			deviceId (0),
			parameterNumber (0),
			midiMin (0),
			midiMax (NOT_SET),
			value (NOT_SET),
			offValue (0),
			onValue (NOT_SET),
			event (0)
		{
			type = (uint8_t) ElectraMessageType::invalid;
			lsbFirst = false;
			signMode = (uint8_t) SignMode::noSign;
			bitWidth = 7;
		}

		Message (uint8_t newDeviceId,
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
				 uint8_t newbitWidth) :
			deviceId (newDeviceId),
			parameterNumber (newParameterNumber),
			midiMin (newMidiMin),
			midiMax (newMidiMax),
			value (newValue),
			offValue (newOffValue),
			onValue (newOnValue),
			data (newData),
			event (0)
		{
			type = (uint8_t) newType;
			lsbFirst = newLsbFirst;
			signMode = (uint8_t) newSignMode;
			bitWidth = newbitWidth;

			if (newType == ElectraMessageType::note)
			{
				offValue = 0;
				onValue = 127; // 0 for note off and 127 for note on.
			}
		}

		void setValue (int16_t newValue)
		{
			value = newValue;
		}

		int16_t getValue (void)
		{
			return (value);
		}

		int16_t getMidiMin (void)
		{
			return (midiMin);
		}

		int16_t getMidiMax (void)
		{
			return (midiMax);
		}

		SignMode getSignMode (void)
		{
			return ((SignMode) signMode);
		}

		Event getEvent (void)
		{
			return ((Event) event);
		}

		void setEvent (Event newEvent)
		{
			event = (uint8_t) newEvent;
		}

		ElectraMessageType getType (void)
		{
			return ((ElectraMessageType) type);
		}

		void setDeviceId (uint8_t newDeviceId)
		{
			deviceId = newDeviceId;
		}

		uint8_t getDeviceId (void)
		{
			return (deviceId);
		}

		uint16_t getParameterNumber (void)
		{
			return (parameterNumber);
		}

		int16_t getOffValue (void)
		{
			return (offValue);
		}

		int16_t getOnValue (void)
		{
			return (onValue);
		}

		bool getLsbFirst (void)
		{
			return (lsbFirst);
		}

		uint8_t getbitWidth (void)
		{
			return (bitWidth);
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

		struct
		{
			uint8_t type : 4;
			bool lsbFirst : 1;
			uint8_t signMode : 2;
			uint8_t bitWidth : 4;
			uint8_t event : 2;
		};
};
