//
// Created by m_ky on 2024/4/22.
//

/**
 * @class   igQtModelDrawWidget
 * @brief   igQtModelDrawWidget's brief
 */

#include <IQWidgets/igQtColorBarWidget.h>
#include <IQWidgets/igQtModelDrawWidget.h>
#include <QtGui/QtGui>

igQtModelDrawWidget::igQtModelDrawWidget(QWidget* parent)
    : igQtRenderWidget(parent) {
    m_ColorBarWidget = new igQtColorBarWidget(this);

    m_ColorBarWidget->setWindowFlag(Qt::WindowStaysOnTopHint);
    m_ColorBarWidget->hide();
    connect(this, &igQtModelDrawWidget::resized, this, [&]() {
        QRect tmp = this->geometry();
        this->m_ColorBarWidget->setLimitPosition(0, 0, tmp.width(),
                                                 tmp.height());
    });

}

igQtModelDrawWidget::~igQtModelDrawWidget() {}
