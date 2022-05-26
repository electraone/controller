#include "Detail.h"
#include "ControlComponent.h"

Detail::Detail(const Control &controlToDisplay, UiDelegate *newDelegate)
    : control(controlToDisplay), delegate(newDelegate)
{
    setBounds(100, 100, 824, 400);
    setName("detail");

    Component *c = ControlComponent::createControlComponent(control);
    c->setBounds(100, 100, 624, 200);
    c->setName("cc in detail");

    if (c) {
        addAndMakeVisible(c);
    }
}

Detail::~Detail()
{
}

void Detail::paint(Graphics &g)
{
    g.fillAll(Colours::black);
    g.setColour(Colours::darkgrey);
    g.drawRect(0, 0, getWidth(), getHeight());
    g.printText(0,
                getHeight() / 2,
                control.getName(),
                TextStyle::largeTransparent,
                getWidth(),
                TextAlign::center);
}

void Detail::resized(void)
{
}
