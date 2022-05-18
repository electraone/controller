#pragma once

#include "ElectraMessage.h"
#include <cstdint>

class Rule
{
public:
    Rule()
        : electraMessageType(ElectraMessageType::invalid),
          parameterNumber(0),
          byte(0),
          parameterBitPosition(0),
          byteBitPosition(0),
          bitWidth(0)
    {
    }

    Rule(ElectraMessageType newElectraMessageType,
         uint16_t newParameterNumber,
         uint16_t newByte,
         uint8_t newParameterBitPosition,
         uint8_t newByteBitPosition,
         uint8_t newBitWidth)
        : electraMessageType(newElectraMessageType),
          parameterNumber(newParameterNumber),
          byte(newByte),
          parameterBitPosition(newParameterBitPosition),
          byteBitPosition(newByteBitPosition),
          bitWidth(newBitWidth)
    {
    }

    ElectraMessageType getType(void)
    {
        return (electraMessageType);
    }

    uint8_t getParameterBitPosition(void)
    {
        return (parameterBitPosition);
    }

    uint8_t getByteBitPosition(void)
    {
        return (byteBitPosition);
    }

    uint8_t getBitWidth(void)
    {
        return (bitWidth);
    }

    uint16_t getParameterNumber(void)
    {
        return (parameterNumber);
    }

    uint16_t getByte(void)
    {
        return (byte);
    }

private:
    ElectraMessageType electraMessageType;
    uint16_t parameterNumber;
    uint16_t byte;
    struct {
        uint8_t parameterBitPosition : 4;
        uint8_t byteBitPosition : 4;
        uint8_t bitWidth : 4;
    };
};
