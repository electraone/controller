#pragma once

#include <stdint.h>

class Input
{
public:
    Input() : handleIndex(0), potId(0)
    {
    }

    Input(uint8_t newHandleIndex, uint8_t newPotId)
        : handleIndex(newHandleIndex), potId(newPotId)
    {
    }

    ~Input() = default;

    uint8_t getPotId(void) const
    {
        return (potId);
    }

    uint8_t getValueId(void) const
    {
        return (handleIndex);
    }

	void print(void) const
	{
	    logMessage("    potId: %d", getPotId());
	    logMessage("    valueId: %d", getValueId());
	}

private:
    uint8_t handleIndex;
    uint8_t potId;
};
