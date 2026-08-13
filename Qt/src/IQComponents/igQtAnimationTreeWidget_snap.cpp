//
// Created by m_ky on 2024/3/19.
//

#include <IQComponents/igQtAnimationTreeWidget_snap.h>
#include <qdebug.h>
igQtAnimationTreeWidget_snap::igQtAnimationTreeWidget_snap(QWidget *parent) : igQtAnimationTreeWidget(parent) {
//    setHeaderLabels({"文件原始时间", "值"});
//
//    qDebug() << "111";
}

void igQtAnimationTreeWidget_snap::initAnimationTreeWidget(std::vector<float> &timeSeq) {
    timeSequence = timeSeq;
    current_Keyframe_index = 0;

    keyframe_sum = static_cast<int>(timeSequence.size());
//    qDebug() << this->topLevelItem(1)->text(0);
    this->topLevelItem(2)->setText(1, QString("%1").arg(keyframe_sum));
    removeAllChildItem(this->topLevelItem(3));
    for(int i = 0; i < keyframe_sum; i ++)
    {
        auto *item = new QTreeWidgetItem();
        item->setText(0, QString("%1").arg(i + 1));
        item->setText(1, QString::asprintf("%.10f", timeSequence[i]));
        this->topLevelItem(3)->addChild(item);
    }
    this->topLevelItem(3)->setText(1, QString("[%1, %2]").arg(timeSequence[0]).arg(timeSequence[keyframe_sum - 1]));
    updateData();
}


