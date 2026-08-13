#include "IQWidgets/ColorManager/igQtCustomColorAreaWidget.h"

igQtCustomColorAreaWidget::igQtCustomColorAreaWidget(QWidget* parent)
    : QWidget(parent)
    , CurrentIndex(0)
{
    // 初始化网格布局
    LayoutGrid = new QGridLayout(this);
    LayoutGrid->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    LayoutGrid->setSpacing(2);
    LayoutGrid->setMargin(2);
    LayoutGrid->setGeometry(QRect(0, 0, width(), height()));

    // 调色板子界面布局
    QString color;
    QStringList strList;
    for (int i = 0; i < 16; i++) {
        igQtBasicColorItem* pItem11 = new igQtBasicColorItem(Qt::white);
        connect(pItem11, SIGNAL(sigItemClicked(const QColor&)), this, SIGNAL(sigColorItemSel(const QColor&)));

       LayoutGrid->addWidget(pItem11, LayoutGrid->count() / 8, LayoutGrid->count() % 8);

        mapIndexToItem.insert(i, pItem11);
    }
}
igQtCustomColorAreaWidget::~igQtCustomColorAreaWidget()
{

}
// 设置自定义颜色
void igQtCustomColorAreaWidget::setGivenColor(const QColor& c)
{
    int iIndex = CurrentIndex % 16;
    igQtBasicColorItem* pCurItem = mapIndexToItem[iIndex];
    pCurItem->setColor(c);
    CurrentIndex++;
}

// 设置自定义项颜色
void igQtCustomColorAreaWidget::setGivenItemColor(int iIndex, const QColor& c)
{
    igQtBasicColorItem* pCurItem = mapIndexToItem[iIndex];
    pCurItem->setColor(c);
}