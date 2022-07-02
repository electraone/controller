#include "SnapshotBankSelection.h"

SnapshotBankSelection::SnapshotBankSelection(
    SnapshotsWindowDelegate &newDelegate,
    uint8_t newBankNumber)
    : delegate(newDelegate), bankNumber(newBankNumber), button{ nullptr }
{
    setName("SnapshotBankSelection");

    for (uint8_t i = 0; i < 12; i++) {
        char label[10];
        snprintf(label, sizeof(label), "Bank %d", i + 1);
        button[i] = new Button();

        if (button[i]) {
            button[i]->setName(label);
            button[i]->setId(i);
            button[i]->setLabel(label);
            button[i]->assignPot(i);
            button[i]->onClick = [this, i]() {
                setActiveBank(i);
                return (true);
            };

            if (bankNumber == i) {
                button[i]->setSelected(true);
            }

            addAndMakeVisible(button[i]);
        }
    }
    setBounds(0, 365, 1024, 210);
}

void SnapshotBankSelection::paint(Graphics &g)
{
    g.dim(0, 0, getWidth(), getHeight(), 0x0000);
    g.dim(0, 0, getWidth(), getHeight(), 0x0001);

    paintTitleBar(g, "Snapshot banks", getWidth(), 0x0002);
}

void SnapshotBankSelection::resized(void)
{
    uint16_t segmentWidth = getWidth() / 6 - 2;
    uint16_t segmentHeight = (getHeight() - topPadding) / 2 - 10;

    for (uint8_t i = 0; i < 12; i++) {
        uint16_t x = (i % 6) * segmentWidth;
        uint16_t y = ((i < 6) ? 0 : segmentHeight + 10) + topPadding;
        button[i]->setBounds(x + 12, y, segmentWidth - 10, segmentHeight);
    }
}

void SnapshotBankSelection::setActiveBank(uint8_t newBankNumber)
{
    uint8_t previousBankNumber = bankNumber;
    bankNumber = newBankNumber;
    delegate.switchSnapshotBank(bankNumber);
    button[previousBankNumber]->setSelected(false);
    button[bankNumber]->setSelected(true);
    repaint();
}

void SnapshotBankSelection::paintTitleBar(Graphics &g,
                                          const char *title,
                                          uint16_t width,
                                          uint32_t colour)
{
    g.setColour(colour);
    g.fillRect(0, 0, width, 25);
    paintIcon(g, 16, 5);
    g.printText(
        40, 8, title, TextStyle::smallTransparent, width, TextAlign::left);
}

void SnapshotBankSelection::paintIcon(Graphics &g, uint16_t x, uint16_t y)
{
    g.setColour(Colours::white);
    g.drawRect(x, y, 10, 13);
    g.fillRect(x + 5, y + 3, 10, 13);
}
