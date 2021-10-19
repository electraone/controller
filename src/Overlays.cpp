#include "Overlays.h"

Overlays::Overlays(MemoryPool &pool) : stringPool(pool), currentOffset(0)
{
}

void Overlays::reset(void)
{
    currentOffset = 0;
}

Overlay *Overlays::create(uint8_t id)
{
    if (id > maxNumOverlays) {
        logMessage("Overlays::create: overlayId must be between 0 and %d",
                   maxNumOverlays);
        return (nullptr);
    }

    overlays[id] = { this, currentOffset };
    return (&overlays[id]);
}

Overlay *Overlays::get(uint8_t id)
{
    return (&overlays[id]);
}

Overlay::Overlay() : offset(0), numItems(0)
{
}

Overlay::Overlay(Overlays *overlayPool, uint16_t offset)
    : offset(offset), numItems(0), overlayPool(overlayPool)
{
}

bool Overlay::addItem(OverlayItem item)
{
    if (overlayPool->currentOffset > Overlays::maxNumOverlayItems) {
        logMessage("addItem: max number of overlay items reached");
        return (false);
    }

    overlayPool->overlayItem[overlayPool->currentOffset] = item;
    overlayPool->currentOffset++;
    numItems++;

    return (true);
}

uint8_t Overlay::getNumItems(void)
{
    return (numItems);
}

address_t Overlay::getItemAddress(uint16_t index)
{
    if (index >= numItems) {
        return (0);
    }
    return (overlayPool->overlayItem[offset + index].item);
}

void Overlay::getItem(uint16_t index,
                      char *buffer,
                      uint8_t MaxNameLength,
                      Bitmap &bitmap)
{
    uint32_t bitmapAddress;

    if (index >= numItems) {
        buffer[0] = '\0';
        return;
    }

    overlayPool->stringPool.getItem(
        getItemAddress(index), buffer, MaxNameLength, &bitmapAddress);
    bitmap.setAddress(bitmapAddress);
}

uint16_t Overlay::getValue(uint16_t index)
{
    if (index >= numItems) {
        return (0);
    }
    return (overlayPool->overlayItem[offset + index].value);
}

uint16_t Overlay::getIndex(uint16_t value)
{
    for (uint16_t index = 0; index < numItems; index++) {
        if (overlayPool->overlayItem[offset + index].value == value) {
            return (index);
        }
    }
    return (notFound);
}

Overlays *Overlay::getOverlays(void)
{
    return (overlayPool);
}
