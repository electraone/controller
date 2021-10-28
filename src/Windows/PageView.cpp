#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"

PageView::PageView(const Preset &preset, uint8_t pageId) : model(preset)
{
    setBounds(0, 0, 1024, 575);
    setName("PageView");

    for (const auto &[id, control] : model.controls) {
        if (control.getPageId() == pageId) {
            Component *c = ControlComponent::createControlComponent(control);

            if (c) {
                addAndMakeVisible(c);
            }
        }
    }

    for (const auto &[id, group] : model.groups) {
        if (group.getPageId() == pageId) {
            GroupControl *g = new GroupControl(group);

            if (g) {
                addAndMakeVisible(g);
            }
        }
    }
}

PageView::~PageView()
{
}

void PageView::paint(Graphics &g)
{
    g.fillAll(Colours::black);

    // \todo bottom bar
    g.setColour(0x2104);
    g.drawLine(5, getHeight() - 20, getWidth() - 10, getHeight() - 20);
    g.printText(0,
                getHeight() - 16,
                "Page 1",
                TextStyle::smallTransparent,
                getWidth(),
                TextAlign::center);
}

void PageView::resized(void)
{
}
