#include "ControllerApp.h"

void Controller::initialise(void)
{
    // Assign the storage drivers.
    stringPool.assignStorageDriver(&Hardware::screen);
    bitmapPool.assignStorageDriver(&Hardware::screen);

    // Load the default preset.
    if (preset.load("ctrlv2/p000.epr") == true) {
        logMessage("Default preset loaded");
    } else {
        // \todo what should be done?
        logMessage("Default preset load failed");
    }

    // Set UI delegate.
    delegate = &mainWindow;

    // Display the preset if valid.
    if (preset.isValid()) {
        displayDefaultPage();
    }
    logMessage("App initialisation completed");
}

void Controller::displayDefaultPage(void)
{
    if (delegate) {
        delegate->setPage(0);
    }
}
