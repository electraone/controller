#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"
#include "BottomBar.h"

PageView::PageView(const Preset &preset,
                   uint8_t newPageId,
                   uint8_t activeControlSetId)
    : model(preset), pageId(newPageId)
{
    setBounds(0, 0, 1024, 575);
    setName("PageView");

    for (const auto &[id, control] : model.controls) {
        if (control.getPageId() == pageId) {
            Component *c = ControlComponent::createControlComponent(control);

            if (control.getControlSetId() != activeControlSetId) {
                c->setDimmed(true);
            }
            else {
              c->assignPot(control.inputs[0].getPotId(),
                           control.values[0].getNumSteps());
            }

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

    BottomBar *bottomBar =
        new BottomBar(model.getName(), model.getPage(pageId).getName());
    bottomBar->setBounds(0, 555, 1024, 19);

    if (bottomBar) {
        addAndMakeVisible(bottomBar);
    }
}

PageView::~PageView()
{
}

void PageView::paint(Graphics &g)
{
    g.fillAll(Colours::black);
}

void PageView::resized(void)
{
}
