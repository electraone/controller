#include "PageSelection.h"
#include "Label.h"
#include "Window.h"
#include "System.h" // we do not want this here
#include "PageView.h"

PageSelection::PageSelection(Pages pages,
                             uint8_t newActivePage,
                             UiDelegate *newDelegate)
    : activePage(newActivePage), delegate(newDelegate)
{
    setBounds(0, 460, 1024, 115);
    setName("pageSelection");

    uint16_t segmentWidth = getWidth() / 6;
    uint16_t segmentHeight = (getHeight() - topPadding) / 2;

    for (uint8_t i = 0; i < 12; i++) {
        Label *label = new Label();

        uint16_t x = (i % 6) * segmentWidth;
        uint16_t y = ((i < 6) ? 0 : segmentHeight + 10) + topPadding;

        label->setBounds({ x, y, segmentWidth, segmentHeight });
        label->setId(i);

        if (pages[i].getHasObjects()) {
            label->setLabel(pages[i].getName());
            label->assignPot(i);
		}
		else {
			label->setLabel(".");
		}

        if (activePage == i) {
            label->setActive(true);
        }

        label->onClick = [this, i]() {
            setActivePage(i);
            return (true);
        };

        addAndMakeVisible(label);
    }
}

PageSelection::~PageSelection()
{
}

void PageSelection::paint(Graphics &g)
{
    g.dim(0, 0, getWidth(), getHeight(), 0x0000);
    g.dim(0, 0, getWidth(), getHeight(), 0x0003);

	g.setColour(0x0003);
	g.fillRect(0, 0, getWidth(), 25);
    setActivePageLabel(activePage);

	g.setColour(Colours::white);
	g.drawRect(16, 5, 10, 13);
	g.fillRect(21, 8, 10, 13);

	g.printText(40,
				8,
				"Pages",
				TextStyle::smallTransparent,
				getWidth(),
				TextAlign::left);


}

void PageSelection::resized(void)
{
}

void PageSelection::setActivePageLabel(uint8_t newActivePage)
{
    for (Component *c : getChildren()) {
        if (Label *l = dynamic_cast<Label *>(c)) {
            // fragile, relies on the ids
            l->setActive((l->getId() == newActivePage) ? true : false);
        }
    }
}

void PageSelection::setActivePage(uint8_t newActivePage)
{
    activePage = newActivePage;

    if (delegate) {
        delegate->setPage(activePage);
    }

	repaint();
}
