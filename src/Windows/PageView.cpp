#include "PageView.h"

PageView::PageView(const Preset &preset,
                   MainDelegate &newDelegate,
                   UiApi &newUiApi,
                   const UiFeatures &newUiFeatures,
                   uint8_t newPageId,
                   uint8_t activeControlSetId)
    : model(preset),
      delegate(newDelegate),
      uiApi(newUiApi),
      uiFeatures(newUiFeatures),
      pageId(newPageId),
      controlSetId(activeControlSetId),
      bottomBar(nullptr)
{
    setName("PageView");
    addGroups(model.groups);
    addControls(model.controls);
    addBottomBar(model.getName(), model.getPage(pageId).getName());
    setBounds(0, 0, 1024, 575);
}

PageView::~PageView(void)
{
    for (const auto &[id, control] : model.controls) {
        if (control.getPageId() == pageId) {
            delegate.removeComponentFromControl(control.getId());
        }
    }
    for (const auto &[id, group] : model.groups) {
        if (group.getPageId() == pageId) {
            delegate.removeComponentFromGroup(group.getId());
        }
    }
    getWindow()->resetActiveTouch();
    parameterMap.disable();
    System::tasks.disableRepaintGraphics();
    System::tasks.clearRepaintGraphics();
    System::tasks.enableRepaintGraphics();
    parameterMap.enable();
}

void PageView::setControlSet(uint8_t newControlSetId)
{
    controlSetId = newControlSetId;

    for (const auto &[id, control] : model.controls) {
        if (control.getPageId() == pageId) {
            auto cc = dynamic_cast<ControlComponent *>(control.getComponent());

            if (cc) {
                configureControl(cc, control);
            }
        }
    }
    for (const auto &[id, group] : model.groups) {
        if (group.getPageId() == pageId) {
            if (auto g = dynamic_cast<GroupControl *>(group.getComponent())) {
                configureGroup(g, group);
            }
        }
    }
    repaint();
}

void PageView::onTouchDown(const TouchEvent &touchEvent)
{
    if (uiFeatures.touchSwitchControlSets) {
        auto y = touchEvent.getScreenY();

        if (y < 240) {
            delegate.switchControlSet(0);
        } else if (y > 400) {
            delegate.switchControlSet(2);
        } else {
            delegate.switchControlSet(1);
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

void PageView::setInfoText(const char *newText)
{
    bottomBar->setInfoText(newText);
    bottomBar->repaint();
}

void PageView::setRamPercentage(uint8_t newPercentage)
{
    bottomBar->setRamPercentage(newPercentage);
    bottomBar->repaint();
}

void PageView::paint(Graphics &g)
{
    g.fillAll(Colours565::black);

    if (uiFeatures.activeControlSetType == ActiveControlSetType::bars) {
        g.setColour(0x7BCF);
        g.fillRect(4, 12 + controlSetId * 179, 5, 168);
        g.fillRect(1014, 12 + controlSetId * 179, 5, 168);
        g.setColour(Colours565::black);
        g.fillRect(6, 14 + controlSetId * 179, 4, 164);
        g.fillRect(1014, 14 + controlSetId * 179, 3, 164);
    } else if (uiFeatures.activeControlSetType
               == ActiveControlSetType::background) {
        g.backdrop(10,
                   12 + controlSetId * 179,
                   1003,
                   168,
                   LookAndFeel::altBackgroundColour);
    }
}

void PageView::resized(void)
{
    bottomBar->setBounds(12, 550, 1000, 22);
}

void PageView::addControls(const Controls &controls)
{
    for (const auto &[id, control] : controls) {
        if (control.getPageId() == pageId) {
            ControlComponent *cc =
                ControlComponent::createControlComponent(control, delegate);

            if (cc) {
                configureControl(cc, control);
                addChildComponent(cc);

                delegate.assignComponentToControl(control.getId(), cc);
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
                configureGroup(g, group);
                addChildComponent(g);
                delegate.assignComponentToGroup(group.getId(), g);
            }
        }
    }
}

void PageView::addBottomBar(const char *presetName, const char *pageName)
{
    bottomBar = new BottomBar(presetName, pageName);

    if (bottomBar) {
        addAndMakeVisible(bottomBar);
    }
}

void PageView::configureGroup(GroupControl *g, const Group &group)
{
    if ((controlSetId * 165) < g->getY()
        && (g->getY() < (controlSetId * 165 + 165))) {
        if (uiFeatures.activeControlSetType == ActiveControlSetType::dim) {
            g->setDimmed(false);
        } else if (uiFeatures.activeControlSetType
                   == ActiveControlSetType::background) {
            g->setUseAltBackground(true);
        }
    } else {
        if (uiFeatures.activeControlSetType == ActiveControlSetType::dim) {
            g->setDimmed(true);
        } else if (uiFeatures.activeControlSetType
                   == ActiveControlSetType::background) {
            g->setUseAltBackground(false);
        }
    }
}

void PageView::configureControl(ControlComponent *cc, const Control &control)
{
    if (control.getControlSetId() == controlSetId) {
        if (uiFeatures.activeControlSetType == ActiveControlSetType::dim) {
            cc->setDimmed(false);
        } else if (uiFeatures.activeControlSetType
                   == ActiveControlSetType::background) {
            cc->setUseAltBackground(true);
        }
    } else {
        if (uiFeatures.activeControlSetType == ActiveControlSetType::dim) {
            cc->setDimmed(true);
        } else if (uiFeatures.activeControlSetType
                   == ActiveControlSetType::background) {
            cc->setUseAltBackground(false);
        }
    }

    if (control.isVisible() && control.getControlSetId() == controlSetId) {
        cc->releasePot();
        cc->assignPot(control.inputs[0].getPotId(),
                      control.values[0].getNumSteps());
    } else {
        cc->releasePot();
    }
}
