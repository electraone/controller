#pragma once

#include "ElectraMessage.h"
#include "Overlays.h"
#include "Message.h"
#include "Macros.h"
#include <stdint.h>

class Control;

// Define a list of all supported value Ids
#define MAX_NUM_VALUE_IDS 13
static const char* valueIds[MAX_NUM_VALUE_IDS] =
{
	"value",
	"attack",
	"decay",
	"sustain",
	"release",
	"l1",
	"r1",
	"l2",
	"r2",
	"l3",
	"r3",
	"l4",
	"r4"
};

class Value2
{
	public:

		Value2 () :
			handle (0),
			defaultValue (0),
			min (0),
			max (NOT_SET),
			overlay (nullptr),
			control (nullptr)
		{
		}

		Value2 (Control* newControl,
			   const char* newValueId,
			   uint8_t newIndex,
			   int16_t newDefaultValue,
			   int16_t newMin,
			   int16_t newMax,
			   uint8_t newOverlayId,
			   Overlay *newOverlay,
			   Message (newMessage),
			   const char* newFormatter,
			   const char* newFunction) :
			control (newControl),
			index (newIndex),
			defaultValue (newDefaultValue),
			min (newMin),
			max (newMax),
			overlayId (newOverlayId),
			message (newMessage),
			formatter (newFormatter),
			function (newFunction)
		{
			// translate the valueId to the numeric handle
			handle = translateId (newValueId);

			// assign the overlay
			if (newOverlay)
			{
				uint16_t numItems = newOverlay->getNumItems ();

				if (numItems == 0)
				{
					overlay = nullptr;
				}
				else
				{
					overlay = newOverlay;
				}
			}
		}

		Control* getControl (void) const
		{
			return (control);
		}

		int16_t getHandle (void) const
		{
			return (handle);
		}

		int16_t getIndex (void) const
		{
			return (index);
		}

		const char* getId (void) const
		{
			return (translateId (handle));
		}

		void setDefault (int16_t newDefaultValue)
		{
			defaultValue = newDefaultValue;
		}

		int16_t getDefault (void) const
		{
			return (defaultValue);
		}

		void setMin (int16_t newMin)
		{
			min = newMin;
		}

		int16_t getMin (void) const
		{
			return (min);
		}

		void setMax (int16_t newMax)
		{
			max = newMax;
		}

		int16_t getMax (void) const
		{
			return (max);
		}

		uint8_t getOverlayId (void) const
		{
			return (overlayId);
		}

		void setOverlayId (uint8_t overlayId)
		{
			Overlays* overlays = overlay->getOverlays ();

			if (overlays)
			{
				overlay = overlays->get (overlayId);
			}
		}

		Overlay *getOverlay (void) const
		{
			return (overlay);
		}

		void callFormat (int16_t value, char *buffer, size_t length)
		{
			/*
			if (L && formatter)
			{
				runFormatter (formatter.c_str (), this, value, buffer, length);
			}
			else
			{
				itoa (value, buffer, 10); // this is a default format
			}
			*/
		}

		void callFunction (int16_t value)
		{
			/*
			if (L && function)
			{
				runFunction (function.c_str (), this, value);
			}
			*/
		}

		static const char* translateId (uint8_t id)
		{
			if ((0 <= id) && (id < MAX_NUM_VALUE_IDS))
			{
				return (valueIds[id]);
			}
			return ("value");
		}

		static uint8_t translateId (const char* handle)
		{
			for (uint8_t i = 0; i < MAX_NUM_VALUE_IDS; i++)
			{
				if (strcmp (handle, valueIds[i]) == 0)
				{
					return (i);
				}
			}
			return (0);
		}

	/*
	 * attributes
	 */
	private:
		uint8_t handle;
		uint8_t index;
		int16_t defaultValue;
		int16_t min;
		int16_t max;
		Overlay* overlay;
		uint8_t overlayId;
		String formatter;
		String function;
		Control* control;

	public:
		Message message;
};
