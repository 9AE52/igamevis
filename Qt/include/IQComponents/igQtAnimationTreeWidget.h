//
// Created by m_ky on 2024/2/28.
//

#pragma once
#include <QTreeWidget>
#include <IQCore/igQtExportModule.h>

class IG_QT_MODULE_EXPORT igQtAnimationTreeWidget : public QTreeWidget{
    Q_OBJECT

public:
    explicit igQtAnimationTreeWidget(QWidget *parent = nullptr);
    ~igQtAnimationTreeWidget() override = default;

protected:

    int current_Keyframe_index{0};
    int keyframe_sum{0};
    std::vector<float> timeSequence;
protected:
    void updateData();
    void handleClick(QTreeWidgetItem* it, int column);
    void removeAllChildItem(QTreeWidgetItem *it);
public:
    virtual void initAnimationTreeWidget(std::vector<float> &timeSeq) = 0;


    int getKeyframeSize();

public slots:
    void updateCurrentKeyframe(int idx);

signals:
    void keyframedChanged(int);
};



