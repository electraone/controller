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
        // Create the root component
        detail = new Detail(control, newDelegate);

        // Determine placement and size of the window
        if (control.getType() == ControlType::fader) {
            setBounds(8, 217, 1008, 210);
        } else if (control.getType() == ControlType::list) {
            uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 677;
            setBounds(xPosition, 22, 336, 556);
        }

        if (detail) {
            setOwnedContent(detail);
            setVisible(true);
        }
    }

public:
    ~DetailWindow() override
    {
        delegate->closeDetail();
    }

    void resized(void) override
    {
        if (control.getType() == ControlType::fader) {
            detail->setBounds(0, 0, 1008, 210);
        } else if (control.getType() == ControlType::list) {
            detail->setBounds(0, 0, 336, 556);
        }
    }

    void onTouchOutside(void) override
    {
        logMessage("closing detail");
        delete this;
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
    const Control &control;
    UiDelegate *delegate;
    Detail *detail;
};
