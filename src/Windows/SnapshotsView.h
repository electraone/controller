#pragma once

#include "Component.h"
#include "SnapsButton.h"
#include "ActionButton.h"
#include "UiDelegate.h"

class SnapshotsView : public Component
{
public:
    enum Mode { load, loadAndStay, save, remove };

    SnapshotsView(UiDelegate &newDelegate,
                  const char *newProjectId,
                  uint8_t newBankNumber,
                  Mode newMode);
    virtual ~SnapshotsView() = default;

    void paint(Graphics &g) override;
    void resized(void) override;
    void setMode(Mode newMode);
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
    static void paintTitleBar(Graphics &g,
                              const char *title,
                              uint16_t width,
                              uint32_t colour);
    static void paintIcon(Graphics &g, uint16_t x, uint16_t y);
    void updateSnapsButtons(void);
    ActionButton *addButton(uint16_t id,
                            const char *label,
                            uint32_t colour,
                            uint32_t colourActive,
                            const Rectangle &bounds);
    void addActionButtons(void);
    void addSnapshotButtons(void);
    void runAction(SnapsButton *button, uint8_t slot);
    void loadSnapshot(SnapsButton *button, uint8_t slot);
    void loadAndStaySnapshot(SnapsButton *button, uint8_t slot);
    void removeSnapshot(uint8_t slot);
    void saveSnapshot(SnapsButton *button, uint8_t slot);

    UiDelegate &delegate;
    const char *currentProjectId;
    uint8_t currentBankNumber;
    Mode mode;
    ActionButton *loadButton;
    ActionButton *loadAndStayButton;
    ActionButton *sendCurrentButton;
    ActionButton *removeButton;
    ActionButton *saveButton;

    SnapsButton *snapsButton[36];

    static constexpr uint8_t topPadding = 50;
};
