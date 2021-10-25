#include "MainComponent.h"
#include "ControlComponent.h"
#include "GroupControl.h"

MainComponent::MainComponent(Preset &preset) : model(preset), currentPage(0)
{
    setBounds(0, 0, 1024, 575);

    assignAllButtons();
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(Colours::black);
	g.setColour(0x2104);
	g.drawLine(5, getHeight() - 20, getWidth() - 10, getHeight() - 20);
    g.printText(0,
                getHeight() - 16,
                "Page 1",
                TextStyle::smallTransparent,
                getWidth(),
                TextAlign::center);
    std::vector<Component *> controls;
}

void MainComponent::resized()
{
}

void MainComponent::onButtonDown(uint8_t buttonId)
{
    logMessage("buttonDown: id=%d", buttonId);
    displayPage(buttonId);
}

void MainComponent::displayPage(uint8_t pageId)
{
    uint32_t tsStart = millis();

    deleteAllChildren();

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

    repaint();

    logMessage("displayPage took: %d", millis() - tsStart);
}
