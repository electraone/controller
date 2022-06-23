#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"

PageView::PageView(const Preset &preset,
                   UiDelegate *newDelegate,
                   const UiFeatures &newUiFeatures,
                   uint8_t newPageId,
                   uint8_t activeControlSetId)
    : model(preset),
      delegate(newDelegate),
      uiFeatures(newUiFeatures),
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
    getWindow()->resetActiveTouch();
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();
    System::tasks.enableRepaintGraphics();
    parameterMap.enable();
}

void PageView::onTouchDown(const TouchEvent &touchEvent)
{
    if (uiFeatures.touchSwitchControlSets) {
        auto y = touchEvent.getScreenY();

        if (y < 240) {
            delegate->setControlSet(0);
        } else if (y > 400) {
            delegate->setControlSet(2);
        } else {
            delegate->setControlSet(1);
        }
    }
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

    if (uiFeatures.activeControlSetType == ActiveControlSetType::bars) {
        g.setColour(0x7BCF);
        g.fillRect(4, 22 + controlSetId * 176, 1, 165);
        g.fillRect(1018, 22 + controlSetId * 176, 1, 165);
    } else if (uiFeatures.activeControlSetType
               == ActiveControlSetType::background) {
        g.backdrop(12,
                   22 + controlSetId * 176,
                   999,
                   166,
                   LookAndFeel::altBackgroundColour);
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
                    c->setDimmed(uiFeatures.activeControlSetType
                                         == ActiveControlSetType::dim
                                     ? true
                                     : false);
                } else {
                    c->setUseAltBackground(
                        uiFeatures.activeControlSetType
                                == ActiveControlSetType::background
                            ? true
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
                // Dim groups that are not in the active control set.
                // This is determined on the group position as the groups are
                // not linked to the control sets (yet).
                if (uiFeatures.activeControlSetType
                    == ActiveControlSetType::dim) {
                    if ((controlSetId * 165) < g->getY()
                        && (g->getY() < (controlSetId * 165 + 165))) {
                        g->setDimmed(false);
                    } else {
                        g->setDimmed(true);
                    }
                }
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
