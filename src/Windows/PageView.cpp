#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"
#include "BottomBar.h"

PageView::PageView(const Preset &preset,
                   uint8_t pageId,
                   uint8_t activeControlSetId)
    : model(preset)
{
    setBounds(0, 0, 1024, 575);
    setName("PageView");

    addControls(model.controls, pageId, activeControlSetId);
    addGroups(model.groups, pageId, activeControlSetId);
    addBottomBar(model.getName(), model.getPage(pageId).getName());
}

PageView::~PageView(void)
{
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();
    System::tasks.enableRepaintGraphics();
    parameterMap.enable();
}

void PageView::paint(Graphics &g)
{
    g.fillAll(Colours::black);
}

void PageView::resized(void)
{
}

void PageView::addControls(const Controls &controls,
                           uint8_t pageId,
                           uint8_t activeControlSetId)
{
    for (const auto &[id, control] : controls) {
        if (control.getPageId() == pageId) {
            Component *c = ControlComponent::createControlComponent(control);

            if (c) {
                if (control.getControlSetId() != activeControlSetId) {
                    c->setDimmed(true);
                } else {
                    c->assignPot(control.inputs[0].getPotId(),
                                 control.values[0].getNumSteps());
                }
                addAndMakeVisible(c);
            }
        }
    }
}

void PageView::addGroups(const Groups &groups,
                         uint8_t pageId,
                         uint8_t activeControlSetId)
{
    for (const auto &[id, group] : groups) {
        if (group.getPageId() == pageId) {
            GroupControl *g = new GroupControl(group);

            if (g) {
                addAndMakeVisible(g);
            }
        }
    }
}

void PageView::addBottomBar(const char *presetName, const char *pageName)
{
    BottomBar *bottomBar = new BottomBar(presetName, pageName);
    bottomBar->setBounds(0, 555, 1024, 19);

    if (bottomBar) {
        addAndMakeVisible(bottomBar);
    }
}
