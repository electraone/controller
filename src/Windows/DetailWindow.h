#pragma once

#include "ParameterMapWindow.h"
#include "Detail.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class DetailWindow : public ParameterMapWindow
{
private:
    DetailWindow(const Control &controlToDisplay, UiDelegate *newDelegate)
        : control(controlToDisplay), delegate(newDelegate), detail(nullptr)
    {
        detail = Detail::createDetail(control, delegate);

        if (detail) {
            setOwnedContent(detail);
        }
        setName("DetailWindow");
        setVisible(true);
    }

public:
    virtual ~DetailWindow() = default;

    void resized(void) override
    {
        detail->setBounds(0, 0, getWidth(), getHeight());
    }

    // API
    void setLocked(void)
    {
        detail->setLocked();
    }

    bool isLocked(void) const
    {
        return (detail->isLocked());
    }

    // Events
    void onTouchOutside(void) override
    {
        delegate->closeDetail();
    }

    void onButtonDown(uint8_t buttonId) override
    {
        if (buttonId == 3) {
            logMessage("callback from detailWindow");
            System::windowManager.listWindows();
        } else if (buttonId == 4) {
            logMessage("callback from detailWindow");
            buttonBroadcaster.listListeners();
        } else if (buttonId == 5) {
            logMessage("closing detail");
            delegate->closeDetail();
        }
    }

    // Factory function
    static DetailWindow *createDetailWindow(const Control &control,
                                            UiDelegate *newDelegate)
    {
        return new DetailWindow(control, newDelegate);
    }

private:
    const Control &control;
    UiDelegate *delegate;
    Detail *detail;
};
