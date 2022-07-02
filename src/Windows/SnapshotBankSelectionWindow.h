#pragma once

#include "Window.h"
#include "SnapshotBankSelection.h"
#include "SnapshotsWindowDelegate.h"

class SnapshotBankSelectionWindow : public Window
{
private:
    SnapshotBankSelectionWindow(SnapshotsWindowDelegate &newDelegate,
                                uint8_t bankNumber)
        : delegate(newDelegate), content(nullptr)
    {
        content = new SnapshotBankSelection(newDelegate, bankNumber);

        if (content) {
            setOwnedContent(content);
        }
        setName("SnapshotBankSelectionWindow");
        setVisible(true);
    }

public:
    virtual ~SnapshotBankSelectionWindow() = default;

    // API

    // Events
    void onButtonUp(uint8_t buttonId)
    {
        if (buttonId == 3) {
            System::windowManager.listWindows();
            buttonBroadcaster.listListeners();
        } else if (buttonId == 5) {
            delegate.closeSnapshotBanks();
        }
    }

    // Factory function
    static SnapshotBankSelectionWindow *
        createSnapshotBankSelectionWindow(SnapshotsWindowDelegate &newDelegate,
                                          uint8_t bankNumber)
    {
        return new SnapshotBankSelectionWindow(newDelegate, bankNumber);
    }

private:
    SnapshotsWindowDelegate &delegate;
    SnapshotBankSelection *content;
};
