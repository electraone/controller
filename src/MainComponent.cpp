#include "MainComponent.h"
#include "helpers.h"

MainComponent::MainComponent()
{
    setBounds(0, 0, 1024, 560);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(ElectraColours::rgb565NumericBlack);
    g.printText(0,
                getHeight() / 2,
                "Midi Controller",
                TextStyle::largeTransparent,
                getWidth(),
                TextAlign::center);
}

void MainComponent::resized()
{
}
