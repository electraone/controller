#pragma once

#include "ElectraMessage.h"
#include <stdint.h>

class Rule
{
public:
    Rule()
        : electraMessageType(ElectraMessageType::invalid),
          parameterNumber(0),
          byte(0),
          pPos(0),
          bPos(0),
          size(0)
    {
    }

    Rule(ElectraMessageType electraMessageType,
         uint16_t parameterNumber,
         uint16_t byte,
         uint8_t pPos,
         uint8_t bPos,
         uint8_t size)
        : electraMessageType(electraMessageType),
          parameterNumber(parameterNumber),
          byte(byte),
          pPos(pPos),
          bPos(bPos),
          size(size)
    {
    }

    ElectraMessageType getType(void)
    {
        return (electraMessageType);
    }

    uint8_t getParameterBitPosition(void)
    {
        return (pPos);
    }

    uint8_t getByteBitPosition(void)
    {
        return (bPos);
    }

    uint8_t getBitWidth(void)
    {
        return (size);
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
        uint8_t pPos : 4;
        uint8_t bPos : 4;
        uint8_t size : 4;
    };
};
