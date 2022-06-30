#pragma once

#include "Component.h"
#include "SnapsButton.h"
#include "ActionButton.h"
#include "UiDelegate.h"

class SnapshotsView : public Component
{
public:
    SnapshotsView(UiDelegate &newDelegate,
                  const char *newProjectId,
                  uint8_t newBankNumber);
    virtual ~SnapshotsView() = default;

    void paint(Graphics &g) override;
    void resized(void) override;

    void snapshotRemoved(uint8_t bankNumber, uint8_t slot);
    void snapshotSaved(uint8_t bankNumber,
                       uint8_t slot,
                       const char *name,
                       uint8_t colour);
    void snapshotsSwapped(uint8_t sourceBankNumber,
                          uint8_t sourceSlot,
                          uint8_t destBankNumber,
                          uint8_t destSlot);

private:
    enum Mode { load, loadAndStay, save, remove };

    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIcon(Graphics &g, uint16_t x, uint16_t y);
    void setMode(Mode newMode);
    void addActionButtons(void);
    void addSnapshotButtons(uint8_t bankNumber);
    void runAction(SnapsButton *button, uint8_t bankNumber, uint8_t slot);
    void loadSnapshot(SnapsButton *button, uint8_t bankNumber, uint8_t slot);
    void loadAndStaySnapshot(SnapsButton *button,
                             uint8_t bankNumber,
                             uint8_t slot);
    void removeSnapshot(uint8_t bankNumber, uint8_t slot);
    void saveSnapshot(SnapsButton *button, uint8_t bankNumber, uint8_t slot);

    UiDelegate &delegate;
    const char *projectId;
    uint8_t bankNumber;
    Mode mode;
    ActionButton *loadButton;
    ActionButton *loadAndStayButton;
    ActionButton *sendCurrentButton;
    ActionButton *removeButton;
    ActionButton *saveButton;

    SnapsButton *snapsButton[36];

    static constexpr uint8_t topPadding = 50;
};
