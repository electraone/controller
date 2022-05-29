#include "Detail.h"
#include "ControlComponent.h"

Detail::Detail(const Control &controlToDisplay, UiDelegate *newDelegate)
    : control(controlToDisplay), delegate(newDelegate)
{
    setName("detail");

    Component *c = ControlComponent::createControlComponent(control, true);

    if (control.getType() == ControlType::fader) {
        setBounds(8, 195, 1008, 210);
        c->setBounds(58, 45, 908, 120);
    } else if (control.getType() == ControlType::list) {
        uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 677;
        setBounds(xPosition, 0, 336, 556);
        c->setBounds(8, 8, getWidth() - 16, getHeight() - 10);
    }

    if (c) {
        addAndMakeVisible(c);
    }
}

void Detail::paint(Graphics &g)
{
    g.fillAll(Colours::black);
    g.setColour(Colours::darkgrey);
    g.drawRect(0, 0, getWidth(), getHeight());
}

void Detail::resized(void)
{
}
