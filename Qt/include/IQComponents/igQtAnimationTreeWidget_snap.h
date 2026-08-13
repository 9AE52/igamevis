//
// Created by m_ky on 2024/3/19.
//

#pragma once
#include "igQtAnimationTreeWidget.h"
#include <IQCore/igQtExportModule.h>
class IG_QT_MODULE_EXPORT igQtAnimationTreeWidget_snap : public igQtAnimationTreeWidget{
    Q_OBJECT

public:
    explicit igQtAnimationTreeWidget_snap(QWidget *parent = nullptr);
    void initAnimationTreeWidget(std::vector<float> &timeSeq) override;

};

