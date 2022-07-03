#pragma once

#include "Window.h"
#include "SnapshotsView.h"
#include "SnapshotBankSelectionWindow.h"
#include "Preset.h"
#include "SnapshotsWindowDelegate.h"
#include "UiDelegate.h"

class SnapshotsWindow : public Window, public SnapshotsWindowDelegate
{
private:
    SnapshotsWindow(UiDelegate &newDelegate,
                    const char *newProjectId,
                    uint8_t newBankNumber)
        : delegate(newDelegate),
          snapshotsView(nullptr),
          snapshotBankSelectionWindow(nullptr),
          currentSnapshotBank(newBankNumber),
          modeIndex(0)
    {
        copyString(projectId, newProjectId, Preset::MaxProjectIdLength);

        snapshotsView = new SnapshotsView(
            newDelegate, newProjectId, newBankNumber, modes[modeIndex]);

        if (snapshotsView) {
            setOwnedContent(snapshotsView);
        }
        setName("SnapshotsWindow");
        setVisible(true);
    }

public:
    virtual ~SnapshotsWindow() = default;

    // API
    void openSnapshotBanks(void) override
    {
        snapshotBankSelectionWindow =
            SnapshotBankSelectionWindow::createSnapshotBankSelectionWindow(
                *this, currentSnapshotBank);
    }

    void closeSnapshotBanks(void) override
    {
        Window::close(snapshotBankSelectionWindow);
        snapshotBankSelectionWindow = nullptr;
    }

    void switchSnapshotBank(uint8_t newBankNumber) override
    {
        delegate.setCurrentSnapshotBank(newBankNumber);
    }

    void switchMode(void) override
    {
        modeIndex = (modeIndex < 3) ? modeIndex + 1 : 0;
        snapshotsView->setMode(modes[modeIndex]);
    }

    void snapshotRemoved(uint8_t bankNumber, uint8_t slot) override
    {
        snapshotsView->snapshotRemoved(bankNumber, slot);
    }

    void snapshotSaved(uint8_t bankNumber,
                       uint8_t slot,
                       const char *name,
                       uint8_t colour) override
    {
        snapshotsView->snapshotSaved(bankNumber, slot, name, colour);
    }

    void snapshotsSwapped(uint8_t sourceBankNumber,
                          uint8_t sourceSlot,
                          uint8_t destBankNumber,
                          uint8_t destSlot) override
    {
        snapshotsView->snapshotsSwapped(
            sourceBankNumber, sourceSlot, destBankNumber, destSlot);
    }

    void snapshotBankSwitched(uint8_t newBankNumber) override
    {
        currentSnapshotBank = newBankNumber;
        SnapshotsView *newSnapshotsView = new SnapshotsView(
            delegate, projectId, newBankNumber, modes[modeIndex]);
        snapshotsView = dynamic_cast<SnapshotsView *>(
            replaceOwnedContent(newSnapshotsView));
    }

    // Events
    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            switchMode();
        } else if (buttonId == 4) {
            delegate.closeSnapshots();
        } else if (buttonId == 5) {
            openSnapshotBanks();
        }
    }

    // Factory function
    static SnapshotsWindow *createSnapshotsWindow(UiDelegate &newDelegate,
                                                  const char *newProjectId,
                                                  uint8_t bankNumber)
    {
        return new SnapshotsWindow(newDelegate, newProjectId, bankNumber);
    }

private:
    UiDelegate &delegate;
    char projectId[Preset::MaxProjectIdLength + 1];
    SnapshotsView *snapshotsView;

    // Sub-windows
    SnapshotBankSelectionWindow *snapshotBankSelectionWindow;

    uint8_t currentSnapshotBank;
    uint8_t modeIndex;
    static constexpr SnapshotsView::Mode modes[4] = {
        SnapshotsView::Mode::load,
        SnapshotsView::Mode::loadAndStay,
        SnapshotsView::Mode::remove,
        SnapshotsView::Mode::save
    };
};
