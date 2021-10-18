#pragma once

#include <stdint.h>
typedef uint8_t handle_t;

class Input
{
	public:
		Input () :
			handleIndex (0), potId (0)
		{
		}

		Input (handle_t newHandleIndex, uint8_t newPotId) :
			handleIndex (newHandleIndex),
			potId (newPotId)
		{
		}

		~Input ()
		{
		}

		handle_t handleIndex;
		uint8_t potId;
};
