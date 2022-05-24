#include "PageView.h"
#include "ControlComponent.h"
#include "GroupControl.h"
#include "BottomBar.h"


PageView::PageView(const Preset &preset,
                   uint8_t pageId,
                   uint8_t activeControlSetId)
    : model(preset), repaintParameterMapTask()
{
    setBounds(0, 0, 1024, 575);
    setName("PageView");

    addControls(model.controls, pageId, activeControlSetId);
    addGroups(model.groups, pageId, activeControlSetId);
    addBottomBar(model.getName(), model.getPage(pageId).getName());

    enableRepaintParameterMap();
}

PageView::~PageView()
{
    System::tasks.disableRepaintGraphics();
    disableRepaintParameterMap();
    System::tasks.flushRepaintGraphics();
    System::tasks.enableRepaintGraphics();
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

void PageView::enableRepaintParameterMap(void)
{
    TaskClassCallback<void(void)>::callbackFunction =
        std::bind(&PageView::repaintParameterMap, this);
    TaskCallback repaintTaskCallback =
        static_cast<TaskCallback>(TaskClassCallback<void(void)>::callback);

    System::tasks.addTask(repaintParameterMapTask);
    repaintParameterMapTask.set(40000, TASK_FOREVER, repaintTaskCallback);
    repaintParameterMapTask.enable();
}

void PageView::disableRepaintParameterMap(void)
{
    System::tasks.deleteTask(repaintParameterMapTask);
}

/*
 * Scheduller task to repaint dirty parameters
 */
void PageView::repaintParameterMap(void)
{
    for (auto &mapEntry : parameterMap.entries) {
        if (mapEntry.dirty) {
#ifdef DEBUG
            logMessage(
                "repaintParameterMap: dirty entry found: device=%d, type=%d, "
                "parameterNumber=%d, midiValue=%d",
                getDeviceId(mapEntry.hash),
                getType(mapEntry.hash),
                getParameterNumber(mapEntry.hash),
                mapEntry.midiValue,
                mapEntry.dirty);
#endif

            for (auto &messageDestination : mapEntry.messageDestination) {
                // \todo this could be made better and more efficient
                Component *c =
                    findChildById(messageDestination.control->getId());

                if (c) {
#ifdef DEBUG
                    logMessage(
                        "repaintParameterMap: repainting component: component: %s, controlId=%d, value=%s",
                        c->getName(),
                        messageDestination.control->getId(),
                        messageDestination.value->getId());
#endif
                    ControlComponent *cc = dynamic_cast<ControlComponent *>(c);

                    if (cc) {
                        cc->onMidiValueChange(
                            *messageDestination.value,
                            mapEntry.midiValue,
                            messageDestination.value->getHandle());
                    }
                }
            }
            mapEntry.dirty = false;
        }
    }
}
