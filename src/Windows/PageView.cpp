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
      activeControlSetType(3),
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

    if (activeControlSetType == 2) {
        g.setColour(0x7BCF);
        g.fillRect(4, 22 + controlSetId * 176, 1, 165);
        g.fillRect(1018, 22 + controlSetId * 176, 1, 165);
    } else if (activeControlSetType == 3) {
        g.backdrop(12, 22 + controlSetId * 176, 999, 166, 0x0081);
    }
}

void PageView::resized(void)
{
}

void PageView::addControls(const Controls &controls)
{
    for (const auto &[id, control] : controls) {
        if (control.getPageId() == pageId) {
            ControlComponent *c =
                ControlComponent::createControlComponent(control, delegate);

            if (c) {
                if (control.getControlSetId() != controlSetId) {
                    c->setDimmed(activeControlSetType == 1 ? true : false);
                } else {
                    c->setUseAltBackground(activeControlSetType == 3 ? true
                                                                     : false);
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
