#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"

PageView::PageView(const Preset &preset,
                   UiDelegate *newDelegate,
                   uint8_t newPageId,
                   uint8_t activeControlSetId)
    : model(preset),
      delegate(newDelegate),
      pageId(newPageId),
      controlSetId(activeControlSetId),
      bottomBar(nullptr)
{
    setBounds(0, 0, 1024, 575);
    setName("PageView");

    addGroups(model.groups);
    addControls(model.controls);
    addBottomBar(model.getName(), model.getPage(pageId).getName());
}

PageView::~PageView(void)
{
    for (const auto &[id, control] : model.controls) {
        if (control.getPageId() == pageId) {
            delegate->removeComponentFromControl(control.getId());
        }
    }
    for (const auto &[id, group] : model.groups) {
        if (group.getPageId() == pageId) {
            delegate->removeComponentFromGroup(group.getId());
        }
    }
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();
    System::tasks.enableRepaintGraphics();
    parameterMap.enable();
}

void PageView::reassignComponent(const Control &control)
{
    Component *c = control.getComponent();

    if (c) {
        c->releasePot();
        if (control.getControlSetId() != controlSetId) {
            c->setDimmed(true);
        } else {
            c->setDimmed(false);
            c->assignPot(control.inputs[0].getPotId(),
                         control.values[0].getNumSteps());
        }
        repaint();
    }
}

void PageView::changePageName(const char *newName)
{
    bottomBar->setPageName(newName);
    bottomBar->repaint();
}

void PageView::paint(Graphics &g)
{
    g.fillAll(Colours::black);
}

void PageView::resized(void)
{
}

void PageView::addControls(const Controls &controls)
{
    for (const auto &[id, control] : controls) {
        if (control.getPageId() == pageId) {
            Component *c = ControlComponent::createControlComponent(control);

            if (c) {
                if (control.getControlSetId() != controlSetId) {
                    c->setDimmed(true);
                } else {
                    c->assignPot(control.inputs[0].getPotId(),
                                 control.values[0].getNumSteps());
                }
                addChildComponent(c);

                delegate->assignComponentToControl(control.getId(), c);
            }
        }
    }
}

void PageView::addGroups(const Groups &groups)
{
    for (const auto &[id, group] : groups) {
        if (group.getPageId() == pageId) {
            GroupControl *g = new GroupControl(group);

            if (g) {
                addAndMakeVisible(g);
                delegate->assignComponentToGroup(group.getId(), g);
            }
        }
    }
}

void PageView::addBottomBar(const char *presetName, const char *pageName)
{
    bottomBar = new BottomBar(presetName, pageName);
    bottomBar->setBounds(0, 555, 1024, 19);

    if (bottomBar) {
        addAndMakeVisible(bottomBar);
    }
}
