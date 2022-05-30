#include "Detail.h"
#include "ControlComponent.h"

Detail::Detail(const Control &controlToDisplay, UiDelegate *newDelegate)
    : control(controlToDisplay), delegate(newDelegate)
{
    setName("detail");

    component = ControlComponent::createControlComponent(control, true);

    if (component) {
        addAndMakeVisible(component);
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
    if (control.getType() == ControlType::fader) {
        component->setBounds(58, 45, 908, 120);
    } else if (control.getType() == ControlType::list) {
        component->setBounds(8, 8, getWidth() - 16, getHeight() - 10);
    }
}
