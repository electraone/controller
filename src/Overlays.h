#pragma once

#include "MemoryPool.h"
#include "BitmapPool.h"


class Overlays;

struct OverlayItem
{
	uint16_t value;
	address_t item;
};

class Overlay
{
	public:
		Overlay ();
		Overlay (Overlays *overlayPool, uint16_t offset);

		bool addItem (OverlayItem item);
		uint8_t getNumItems (void);

		void getItem (uint16_t index, char *buffer, uint8_t MaxNameLength,
			  Bitmap &bitmap);
		uint16_t getValue (uint16_t index);
		uint16_t getIndex (uint16_t value);
		Overlays* getOverlays (void);

		static const uint16_t notFound = 65535;

	private:
		uint16_t offset;
		uint16_t numItems;
		Overlays *overlayPool;

		address_t getItemAddress (uint16_t index);
};

class Overlays
{
	public:
		Overlays (MemoryPool& pool);

		Overlay *create (uint8_t id);
		Overlay *get (uint8_t id);
		void reset (void);

	private:
		static const uint16_t maxNumOverlayItems = 25;
		static const uint8_t maxNumOverlays = 5;

		MemoryPool& stringPool;
		uint16_t currentOffset;
		OverlayItem overlayItem[maxNumOverlayItems];
		Overlay overlays[maxNumOverlays];

		friend class Overlay;
};
