#pragma once

#include "Window.h"
#include "Detail.h"
#include "Model/Page.h"
#include "UiDelegate.h"

class DetailWindow : public Window
{
public:
    DetailWindow(const Control &control, UiDelegate *newDelegate)
    {
        Detail *detail = new Detail(control, newDelegate);

        if (detail) {
            setOwnedContent(detail);
            setVisible(true);
        }
    }

    ~DetailWindow()
    {
        if (onWindowClose) {
            onWindowClose();
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

    // \todo Do we need this?
    std::function<void(void)> onWindowClose;

private:
};
