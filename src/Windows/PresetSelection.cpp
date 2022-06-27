#include "PresetSelection.h"

PresetSelection::PresetSelection(const Presets &newPresets,
                                 const PresetBanks &newPresetBanks,
                                 UiDelegate &newDelegate)
    : presets(newPresets),
      presetBanks(newPresetBanks),
      delegate(newDelegate),
      active(presets.getCurrentSlot()),
      activeBank(delegate.getCurrentPresetBank()),
      bankButton{ nullptr },
      button{ nullptr }
{
    setName("PresetSelection");

    for (uint8_t i = 0; i < 6; i++) {
        bankButton[i] = new BankButton();

        if (bankButton[i]) {
            bankButton[i]->setId(i + 100); // offset the id
            bankButton[i]->setLabel(presetBanks[i].name);

            auto colour =
                ElectraColours::getNumericRgb565(presetBanks[i].colour);

            bankButton[i]->setColours(Colours::darker(colour, 0.1f),
                                      Colours::darker(colour, 0.3f));

            if (i == activeBank) {
                bankButton[i]->setSelected(true);
            } else {
                bankButton[i]->setSelected(false);
            }

            bankButton[i]->onClick = [this, i]() {
                setActiveBank(i);
            };
            addAndMakeVisible(bankButton[i]);
        }
    }

    for (uint8_t i = 0; i < 12; i++) {
        button[i] = new PresetButton();

        if (button[i]) {
            button[i]->setId(i);
            button[i]->setLabel(presets.getPresetName(i));
            button[i]->assignPot(i);

            button[i]->onClick = [this, i]() {
                delegate.switchPreset(presets.getCurrentBankNumber(), i);
                delegate.closePresetSelection();
                return (true);
            };

            if (active == i) {
                button[i]->setSelected(true);
            }
            addAndMakeVisible(button[i]);
        }
    }
    setBounds(0, 265, 1024, 310);
}

void PresetSelection::paint(Graphics &g)
{
    setActiveLabel(active);

    g.dim(0, 0, getWidth(), getHeight(), 0x0000);
    g.dim(0, 0, getWidth(), getHeight(), 0x0020);

    paintTitleBar(g, "Presets", getWidth(), 0x1080);
}

void PresetSelection::resized(void)
{
    uint16_t segmentWidth = getWidth() / 6 - 2;
    uint16_t segmentHeight = (getHeight() - topPadding - 100) / 2 - 10;

    for (uint8_t i = 0; i < 6; i++) {
        uint16_t x = (i % 6) * segmentWidth;
        uint16_t y = ((i < 6) ? 0 : segmentHeight + 10) + topPadding;
        bankButton[i]->setBounds(x + 12, y, segmentWidth - 10, segmentHeight);
    }

    for (uint8_t i = 0; i < 12; i++) {
        uint16_t x = (i % 6) * segmentWidth;
        uint16_t y = ((i < 6) ? 0 : segmentHeight + 10) + topPadding;
        button[i]->setBounds(x + 12, y + 100, segmentWidth - 10, segmentHeight);
    }
}

void PresetSelection::setActiveLabel(uint8_t newActive)
{
    for (Component *c : getChildren()) {
        if (PresetButton *l = dynamic_cast<PresetButton *>(c)) {
            // fragile, relies on the ids
            l->setActive((l->getId() == newActive) ? true : false);
        }
    }
}

void PresetSelection::setActiveBank(uint8_t newActiveBank)
{
    bankButton[activeBank]->setSelected(false);
    activeBank = newActiveBank;
    delegate.switchPresetBank(activeBank);
    for (uint8_t i = 0; i < 12; i++) {
        button[i]->setLabel(presets.getPresetName(i));
    }
    bankButton[activeBank]->setSelected(true);
    repaint();
}

void PresetSelection::paintTitleBar(Graphics &g,
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

void PresetSelection::paintIcon(Graphics &g, uint16_t x, uint16_t y)
{
    g.setColour(Colours::white);
    g.drawRect(x, y, 10, 13);
    g.fillRect(x + 5, y + 3, 10, 13);
}
