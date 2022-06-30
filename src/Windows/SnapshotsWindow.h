#pragma once

#include "Window.h"
#include "SnapshotsView.h"
#include "UiDelegate.h"

class SnapshotsWindow : public Window
{
private:
    SnapshotsWindow(UiDelegate &newDelegate, const char *newProjectId)
        : delegate(newDelegate), snapshotsView(nullptr)
    {
        snapshotsView = new SnapshotsView(newDelegate, newProjectId, 0);

        if (snapshotsView) {
            setOwnedContent(snapshotsView);
            setVisible(true);
        }

        setName("SnapshotsWindow");
    }

public:
    virtual ~SnapshotsWindow() = default;

    void snapshotRemoved(uint8_t bankNumber, uint8_t slot)
    {
        snapshotsView->snapshotRemoved(bankNumber, slot);
    }

    void snapshotSaved(uint8_t bankNumber,
                       uint8_t slot,
                       const char *name,
                       uint8_t colour)
    {
        snapshotsView->snapshotSaved(bankNumber, slot, name, colour);
    }

    void snapshotsSwapped(uint8_t sourceBankNumber,
                          uint8_t sourceSlot,
                          uint8_t destBankNumber,
                          uint8_t destSlot)
    {
        snapshotsView->snapshotsSwapped(
            sourceBankNumber, sourceSlot, destBankNumber, destSlot);
    }

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 4) {
            delegate.closeSnapshots();
        }
    }

    static SnapshotsWindow *createSnapshotsWindow(UiDelegate &newDelegate,
                                                  const char *newProjectId)
    {
        SnapshotsWindow *sw = new SnapshotsWindow(newDelegate, newProjectId);

        if (sw) {
            sw->repaint();
        }

        return (sw);
    }

private:
    UiDelegate &delegate;
    SnapshotsView *snapshotsView;
};
