#pragma once

#include "Component.h"
#include "Model/Page.h"
#include "UiDelegate.h"
#include "Label.h"

class PageSelection : public Component
{
public:
    PageSelection(Pages newPages,
                  uint8_t newActivePage,
                  UiDelegate *newDelegate);
    virtual ~PageSelection() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void setActivePageLabel(uint8_t newActivePage);
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIconPages(Graphics &g, uint16_t x, uint16_t y);

    // Delegated functions ---------------------------------------------------
    void setActivePage(uint8_t newActivePage);

    Pages pages;
    uint8_t activePage;
    UiDelegate *delegate;

    Label *label[12];

    static constexpr uint8_t topPadding = 50;
};
