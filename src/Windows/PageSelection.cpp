#include "PageSelection.h"
#include "Window.h"
#include "PageView.h"

PageSelection::PageSelection(Pages newPages,
                             uint8_t newActivePage,
                             UiDelegate *newDelegate)
    : pages(newPages),
      activePage(newActivePage),
      delegate(newDelegate),
      pageButton{ nullptr }
{
    setName("pageSelection");

    for (uint8_t i = 0; i < 12; i++) {
        uint8_t pageId = i + 1;
        pageButton[i] = new PageButton();

        if (pageButton[i]) {
            pageButton[i]->setId(pageId);

            if (pages[pageId].getHasObjects()) {
                pageButton[i]->setLabel(pages[pageId].getName());
                pageButton[i]->assignPot(i);
                pageButton[i]->onClick = [this, pageId]() {
                    setActivePage(pageId);
                    return (true);
                };
            }

            if (activePage == pageId) {
                pageButton[i]->setSelected(true);
            }

            addAndMakeVisible(pageButton[i]);
        }
    }
    setBounds(0, 365, 1024, 210);
}

void PageSelection::paint(Graphics &g)
{
    setActivePageLabel(activePage);

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
        pageButton[i]->setBounds(x + 12, y, segmentWidth - 10, segmentHeight);
    }
}

void PageSelection::setActivePageLabel(uint8_t newActivePage)
{
    for (Component *c : getChildren()) {
        if (PageButton *l = dynamic_cast<PageButton *>(c)) {
            // fragile, relies on the ids
            l->setActive((l->getId() == newActivePage) ? true : false);
        }
    }
}

void PageSelection::setActivePage(uint8_t newActivePage)
{
    pageButton[activePage - 1]->setSelected(false);

    activePage = newActivePage;

    if (delegate) {
        delegate->setPage(activePage,
                          pages.at(activePage).getDefaultControlSetId());
    }

    pageButton[activePage - 1]->setSelected(true);

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
