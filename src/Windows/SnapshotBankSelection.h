#pragma once

#include "Component.h"
#include "SnapshotsWindowDelegate.h"
#include "Button.h"

class SnapshotBankSelection : public Component
{
public:
    SnapshotBankSelection(SnapshotsWindowDelegate &newDelegate,
                          uint8_t newBankNumber);
    virtual ~SnapshotBankSelection() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

private:
    void setActiveBank(uint8_t newBankNumber);
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIcon(Graphics &g, uint16_t x, uint16_t y);

    SnapshotsWindowDelegate &delegate;
    uint8_t bankNumber;
    Button *button[12];

    static constexpr uint8_t topPadding = 50;
};
