#pragma once

#include "ListControl.h"

// \todo height of the item is fixed to 80. Changing it will break the layout
class ListDetailControl : public ListControl
{
public:
    explicit ListDetailControl(const Control &control, UiDelegate *newDelegate)
        : ListControl(control),
          valueChosen(false),
          offsetY(0),
          prevTouchY(0),
          listItemWidth(getWidth()),
          listItemHeight(80),
          maxItemsInViewport(7),
          itemTop(offsetY / listItemHeight),
          delegate(newDelegate)
    {
    }

    ~ListDetailControl() = default;

    void paint(Graphics &g) override
    {
        g.fillAll(Colours::black);
        paintListItems(g, offsetY);
        g.copy(SDRAM_PAGE_4,
               0,
               (offsetY % listItemHeight),
               g.getActiveBufferAddress(),
               0,
               0,
               listItemWidth,
               getHeight() - 5);
    }

    void resized(void) override
    {
        listItemWidth = getWidth();
        maxItemsInViewport = (getHeight() / listItemHeight) + 1;
    }

    void onTouchDown(const TouchEvent &touchEvent) override
    {
        prevTouchY = touchEvent.getY();
    }

    void onTouchMove(const TouchEvent &touchEvent) override
    {
        auto overlay = getList();
        uint16_t numItems = overlay->getNumItems();

        if (numItems < maxItemsInViewport) {
            return;
        }

        if (prevTouchY != touchEvent.getY()) {
            offsetY += prevTouchY - touchEvent.getY();

            if (offsetY < 0) {
                offsetY = 0;
            } else if (offsetY > ((numItems * listItemHeight)
                                  - (listItemHeight * maxItemsInViewport))) {
                offsetY = (numItems * listItemHeight)
                          - (listItemHeight * maxItemsInViewport);
            }

            prevTouchY = touchEvent.getY();
            itemTop = offsetY / listItemHeight;

            repaint();
        }
    }

    void onTouchClick(const TouchEvent &touchEvent) override
    {
        int16_t newIndex = (offsetY + touchEvent.getY()) / listItemHeight;
        emitValueChange(newIndex, control.getValue(0));

        valueChosen = true;
    }

    void onTouchUp(const TouchEvent &touchEvent) override
    {
        if (valueChosen) {
            if (!delegate->isDetailLocked()) {
                delegate->closeDetail();
            }
        }
    }

    void onTouchLongHold(const TouchEvent &touchEvent)
    {
    }

    void onPotTouchUp(const PotEvent &potEvent) override
    {
        if (!delegate->isDetailLocked()) {
            delegate->closeDetail();
        }
    }

    void onMidiValueChange(const ControlValue &value,
                           int16_t midiValue,
                           uint8_t handle = 1) override
    {
        if (auto list = getList()) {
            int16_t index = list->getIndexByValue(midiValue);

            if (index >= 0) {
                setIndex(index);
                determineOffsetY();
            }
        }
    }

private:
    void paintListItems(Graphics &g, int16_t offset)
    {
        auto overlay = getList();

        uint16_t firstItem = offset / listItemHeight;
        uint16_t j = 0;
        uint16_t numItems = overlay->getNumItems();
        uint32_t writeAddress = g.getActiveBufferAddress();

        // set for the list rendering memory page
        auto virtualWindowHeight = listItemHeight * (maxItemsInViewport + 1);
        auto origX = g.getActiveWindowX();
        auto origY = g.getActiveWindowY();
        auto origWidth = g.getActiveWindowWidth();
        auto origHeight = g.getActiveWindowHeight();
        g.setCanvasAddress(SDRAM_PAGE_4);
        g.setActiveWindowPosition(0, 0);
        g.setActiveWindowSize(1024, virtualWindowHeight);

        g.setColour(Colours::black);
        g.fillRect(0, 0, listItemWidth, virtualWindowHeight);

        for (uint8_t i = firstItem; i < numItems; i++) {
            if (j > maxItemsInViewport) {
                break;
            }

            auto listItem = overlay->getByIndex(i);

            g.setColour(i % 2 ? ElectraColours::getNumericRgb565Darker(
                            control.getColour())
                              : ElectraColours::getNumericRgb565Dark(
                                  control.getColour()));
            g.fillRect(0, j * listItemHeight + 0, listItemWidth, 60);
            g.printText(0,
                        j * listItemHeight + 25,
                        listItem.getLabel(),
                        TextStyle::mediumTransparent,
                        listItemWidth,
                        TextAlign::center);

            if (listItem.isBitmapEmpty() == false) {
                Bitmap bitmap = listItem.getBitmap();
                Hardware::memory.bitmapPool.paint(
                    bitmap,
                    28,
                    j * listItemHeight + 22,
                    ElectraColours::getNumericRgb565(control.getColour()),
                    SDRAM_PAGE_4);
            }

            if (index == i) {
                g.setColour(
                    ElectraColours::getNumericRgb565(control.getColour()));
                g.drawRect(0, j * listItemHeight + 0, listItemWidth, 60);
                g.drawRect(1, j * listItemHeight + 1, listItemWidth - 2, 58);
            }
            j++;
        }

        g.setCanvasAddress(writeAddress);
        g.setActiveWindowPosition(origX, origY);
        g.setActiveWindowSize(origWidth, origHeight);
    }

    void determineOffsetY(void)
    {
        auto overlay = getList();
        uint16_t numItems = overlay->getNumItems();

        if (numItems < maxItemsInViewport) {
            offsetY = 0;
        } else if (index <= itemTop
                   || index >= (itemTop + maxItemsInViewport)) {
            if (index < itemTop) {
                itemTop = index;
            } else if (index >= (itemTop + maxItemsInViewport)) {
                itemTop = index - maxItemsInViewport + 1;
            }
            offsetY = itemTop * listItemHeight;
        }
    }

    std::function<void(void)> emit;

private:
    bool valueChosen;
    int16_t offsetY;
    int16_t prevTouchY;

    uint16_t listItemWidth;
    uint16_t listItemHeight;
    uint16_t maxItemsInViewport;

    uint16_t itemTop;

    UiDelegate *delegate;
};
