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
            setVisible(true);
        }
    }

public:
    virtual ~DetailWindow() = default;

    void resized(void) override
    {
        detail->setBounds(0, 0, getWidth(), getHeight());
    }

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

    void setLocked(void)
    {
        detail->setLocked();
    }

    bool isLocked(void)
    {
        return (detail->isLocked());
    }

    static DetailWindow *createDetailWindow(const Control &control,
                                            UiDelegate *newDelegate)
    {
        DetailWindow *detailWindow = new DetailWindow(control, newDelegate);

        if (detailWindow) {
            detailWindow->setName("detailWindow");
            detailWindow->repaint();
        }
        return (detailWindow);
    }

private:
    const Control &control;
    UiDelegate *delegate;
    Detail *detail;
};
