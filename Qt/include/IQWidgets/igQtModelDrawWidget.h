#pragma once

#include "igQtRenderWidget.h"
#include "igQtColorBarWidget.h"
class igQtModelDrawWidget : public igQtRenderWidget {
    Q_OBJECT
public:
    igQtModelDrawWidget(QWidget* parent = nullptr);
    ~igQtModelDrawWidget() override;

    igQtColorBarWidget* getColorBarWidget() { 
        return m_ColorBarWidget;
    }

protected:

    /* 没用可以删了，功能整合到 igQtRenderWight 上 */

private:
    igQtColorBarWidget* m_ColorBarWidget;
};
