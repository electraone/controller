#include "PageSelection.h"

PageSelection::PageSelection(Pages newPages,
                             uint8_t newActivePage,
                             UiApi &newUiApi)
    : pages(newPages),
      activePage(newActivePage),
      uiApi(newUiApi),
      button{ nullptr }
{
    setName("pageSelection");

    for (uint8_t i = 0; i < 12; i++) {
        uint8_t pageId = i + 1;
        button[i] = new Button();

        if (button[i]) {
            button[i]->setId(pageId);

            if (pages[pageId].getHasObjects()) {
                button[i]->setLabel(pages[pageId].getName());
                button[i]->assignPot(i);
                button[i]->onClick = [this, pageId]() {
                    setActivePage(pageId);
                    return (true);
                };
            }

            if (activePage == pageId) {
                button[i]->setSelected(true);
            }

            addAndMakeVisible(button[i]);
        }
    }
    setBounds(0, 365, 1024, 210);
}

void PageSelection::paint(Graphics &g)
{
    g.dim(0, 0, getWidth(), getHeight(), 0x0000);
    g.dim(0, 0, getWidth(), getHeight(), 0x0001);

    paintTitleBar(g, "Pages", getWidth(), 0x0002);
}

void PageSelection::resized(void)
{
    uint16_t segmentWidth = getWidth() / 6 - 2;
    uint16_t segmentHeight = (getHeight() - topPadding) / 2 - 10;

    for (uint8_t i = 0; i < 12; i++) {
        uint16_t x = (i % 6) * segmentWidth;
        uint16_t y = ((i < 6) ? 0 : segmentHeight + 10) + topPadding;
        button[i]->setBounds(x + 12, y, segmentWidth - 10, segmentHeight);
    }
}

void PageSelection::setActivePage(uint8_t newActivePage)
{
    uint8_t previousActivePage = activePage;
    activePage = newActivePage;
    uiApi.switchPage(activePage);
    button[previousActivePage - 1]->setSelected(false);
    button[activePage - 1]->setSelected(true);
    repaint();
}

void PageSelection::paintTitleBar(Graphics &g,
                                  const char *title,
                                  uint16_t width,
                                  uint32_t colour)
{
    g.setColour(colour);
    g.fillRect(0, 0, width, 25);
    paintIconPages(g, 16, 5);
    g.printText(
        40, 8, title, TextStyle::smallTransparent, width, TextAlign::left);
}

void PageSelection::paintIconPages(Graphics &g, uint16_t x, uint16_t y)
{
    g.setColour(Colours::white);
    g.drawRect(x, y, 10, 13);
    g.fillRect(x + 5, y + 3, 10, 13);
}
