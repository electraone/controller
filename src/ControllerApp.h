#include "App.h"
#include "MainWindow.h"
#include "PageView.h"
#include "Preset.h"

#include "MemoryPool.h"
#include "BitmapPool.h"

#include "Ui.h"
#include "UiDelegate.h"

extern MemoryPool stringPool;
extern BitmapPool bitmapPool;

class Controller : public App
{
public:
    Controller() : mainWindow(MainWindow(preset)), delegate(nullptr)
    {
    }

    const char *getApplicationName(void) const override
    {
        return ("Controller");
    }

    const char *getApplicationVersion(void) const override
    {
        return ("2.2.0");
    }

    const char *getApplicationSandbox(void) const override
    {
        return ("ctrlv2");
    }

    void initialise(void) override;

private:
    // Delegated functions ---------------------------------------------------
    void displayDefaultPage(void);

    MainWindow mainWindow;
    Preset preset;
    UiDelegate *delegate;
};

// This macro instructs main() routine to launch the app.
START_ELECTRA_APPLICATION(Controller)
