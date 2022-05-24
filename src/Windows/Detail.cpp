#include "Detail.h"

Detail::Detail(const Control &controlToDisplay, UiDelegate *newDelegate)
    : control(controlToDisplay), delegate(newDelegate)
{
    setBounds(100, 100, 824, 400);
    setName("detail");
}

Detail::~Detail()
{
}

void Detail::paint(Graphics &g)
{
    g.fillAll(Colours::red);
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
