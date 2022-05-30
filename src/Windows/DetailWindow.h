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
        detail = new Detail(control, newDelegate);

        setWindowBounds(control.getType());

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
        detail->setBounds(0, 0, getWidth(), getHeight());
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
    void setWindowBounds(ControlType type)
    {
        // Determine placement and size of the window
        if (type == ControlType::fader) {
            setBounds(8, 217, 1008, 210);
        } else if (type == ControlType::list) {
            uint16_t xPosition = (control.getBounds().getX() > 510) ? 8 : 677;
            setBounds(xPosition, 22, 336, 556);
        } else if (type == ControlType::adsr) {
            setBounds(100, 92, 800, 430);
        } else if (type == ControlType::adr) {
            setBounds(100, 92, 800, 430);
        } else if (type == ControlType::dx7envelope) {
            setBounds(100, 92, 800, 430);
        }
    }

    const Control &control;
    UiDelegate *delegate;
    Detail *detail;
};
