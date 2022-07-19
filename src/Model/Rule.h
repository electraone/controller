#pragma once

#include "ElectraMessage.h"
#include <cstdint>

class Rule
{
public:
    Rule()
        : electraMessageType((uint8_t)ElectraMessageType::invalid),
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
        : electraMessageType((uint8_t)newElectraMessageType),
          parameterNumber(newParameterNumber),
          byte(newByte),
          parameterBitPosition(newParameterBitPosition),
          byteBitPosition(newByteBitPosition),
          bitWidth(newBitWidth)
    {
    }

    ElectraMessageType getType(void) const
    {
        return ((ElectraMessageType)electraMessageType);
    }

    uint8_t getParameterBitPosition(void) const
    {
        return (parameterBitPosition);
    }

    uint8_t getByteBitPosition(void) const
    {
        return (byteBitPosition);
    }

    uint8_t getBitWidth(void) const
    {
        return (bitWidth);
    }

    uint16_t getParameterNumber(void) const
    {
        return (parameterNumber);
    }

    uint16_t getByte(void) const
    {
        return (byte);
    }

private:
    uint16_t parameterNumber;
    uint16_t byte;
    struct {
        uint8_t electraMessageType : 4;
        uint8_t parameterBitPosition : 4;
        uint8_t byteBitPosition : 4;
        uint8_t bitWidth : 4;
    };
};

typedef std::vector<Rule> Rules;
