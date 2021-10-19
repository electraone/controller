#include "SignMode.h"
#include <string.h>
#include "helpers.h"
#include "Arduino.h"

SignMode translateSignMode(const char *signModeText)
{
    if (signModeText) {
        if (strcmp(signModeText, "twosComplement") == 0) {
            return (SignMode::twosComplement);
        } else if (strcmp(signModeText, "signBit") == 0) {
            return (SignMode::signBit);
        }
    }

    return (SignMode::noSign);
}

uint16_t getTwosComplementMidiValue(uint8_t bitWidth,
                                    int16_t value,
                                    int16_t minValue,
                                    int16_t maxValue)
{
    uint16_t midiValue = 0;
    int16_t range = getRange(bitWidth);

    if (value < 0) {
        midiValue = range - abs(value);
    } else {
        midiValue = value;
    }

    //logMessage ("twosComplement: value=%d results in midiValue=%d", value, midiValue);
    return (midiValue);
}

uint16_t getSignBitMidiValue(uint8_t bitWidth,
                             int16_t value,
                             int16_t minValue,
                             int16_t maxValue)
{
    uint16_t midiValue = 0;
    int16_t signBit = getRange(bitWidth) / 2;

    if (value < 0) {
        midiValue = signBit + abs(value);
    } else {
        midiValue = value;
    }

    //logMessage ("signBit: value=%d results in midiValue=%d", value, midiValue);

    return (midiValue);
}

uint16_t getUnsignedMidiValue(int16_t value,
                              int16_t minValue,
                              int16_t maxValue,
                              uint16_t midiMin,
                              uint16_t midiMax)
{
    // Add 1 to midiMax when parity differs
    uint16_t midiValue = map(
        value,
        minValue,
        maxValue,
        midiMin,
        midiMax + (((maxValue - minValue) % 2) != ((midiMax - midiMin) % 2)));

    /* Make sure the output midiValue is within the range given by midiMin and
	 * midiMax
	 */
    midiValue = constrain(midiValue, midiMin, midiMax);
    // logMessage ("unsigned: value=%d, minValue=%d, maxValue=%d, midiMin=%d,"
    // " midiMax=%d results in midiValue=%d", value, minValue, maxValue, midiMin, midiMax, midiValue);
    return (midiValue);
}

uint16_t translateValueToMidiValue(SignMode signMode,
                                   uint8_t bitWidth,
                                   int16_t value,
                                   int16_t minValue,
                                   int16_t maxValue,
                                   uint16_t midiMin,
                                   uint16_t midiMax)
{
    uint16_t midiValue = 0;

    if (signMode == SignMode::twosComplement) {
        midiValue =
            getTwosComplementMidiValue(bitWidth, value, minValue, maxValue);
    } else if (signMode == SignMode::signBit) {
        midiValue = getSignBitMidiValue(bitWidth, value, minValue, maxValue);
    } else {
        midiValue =
            getUnsignedMidiValue(value, minValue, maxValue, midiMin, midiMax);
    }

    return (midiValue);
}

int16_t getTwosComplementValue(uint8_t bitWidth,
                               uint16_t midiValue,
                               int16_t minValue,
                               int16_t maxValue)
{
    int16_t value = 0;

    uint16_t range = getRange(bitWidth);

    if (midiValue >= (range / 2)) {
        value = midiValue - range;
    } else {
        value = midiValue;
    }

    return (value);
}

int16_t getSignBitValue(uint8_t bitWidth,
                        uint16_t midiValue,
                        int16_t minValue,
                        int16_t maxValue)
{
    int16_t value = 0;

    int16_t signBit = getRange(bitWidth) / 2;

    if (midiValue >= signBit) {
        value = signBit - midiValue;
    } else {
        value = midiValue;
    }

    return (value);
}

int16_t getUnsignedValue(uint16_t midiValue,
                         uint16_t midiMin,
                         uint16_t midiMax,
                         int16_t minValue,
                         int16_t maxValue)
{
    /* First make sure the midiValue is within boundaries given by midiMin and
	 * midiMax. This is needed, because the MIDI values from outside can be outside
	 * the permitted range
	 */
    midiValue = constrain(midiValue, midiMin, midiMax);
    return (map(midiValue, midiMin, midiMax, minValue, maxValue));
}

int16_t translateMidiValueToValue(SignMode signMode,
                                  uint8_t bitWidth,
                                  uint16_t midiValue,
                                  uint16_t midiMin,
                                  uint16_t midiMax,
                                  int16_t minValue,
                                  int16_t maxValue)
{
    int16_t value = 0;

    /* TODO: for signed values, check that the midi value is within the range
	 * given by the bitWidth
	 */
    if (signMode == SignMode::twosComplement) {
        value = getTwosComplementValue(bitWidth, midiValue, minValue, maxValue);
    } else if (signMode == SignMode::signBit) {
        value = getSignBitValue(bitWidth, midiValue, minValue, maxValue);
    } else {
        value =
            getUnsignedValue(midiValue, midiMin, midiMax, minValue, maxValue);
    }

    // And make sure the resulting value is also within the min and max range
    value = constrain(value, minValue, maxValue);

    return (value);
}
