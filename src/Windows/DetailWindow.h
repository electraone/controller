#pragma once

#include "ParameterMapWindow.h"
#include "Detail.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class DetailWindow : public ParameterMapWindow
{
public:
    DetailWindow(const Control &control, UiDelegate *newDelegate)
        : delegate(newDelegate)
    {
        Detail *detail = new Detail(control, newDelegate);

        if (detail) {
            setOwnedContent(detail);
            setVisible(true);
        }

        setName("detailWindow");
    }

    ~DetailWindow()
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
            logMessage("callback from detailWindow");
            delete this;
        }
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
    UiDelegate *delegate;
};
