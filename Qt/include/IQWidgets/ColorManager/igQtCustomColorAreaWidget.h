#ifndef IGQTCUSTOMCOLORAREAWIDGET_H
#define IGQTCUSTOMCOLORAREAWIDGET_H

#include <IQCore/igQtExportModule.h>
#include <IQWidgets/ColorManager/igQtBasicColorItem.h>
#include <QPainter>
#include <QWidget>
#include <QColor>
#include <QGridLayout>
class IG_QT_MODULE_EXPORT  igQtCustomColorAreaWidget : public QWidget {
	Q_OBJECT

public:
	igQtCustomColorAreaWidget(QWidget* parent = nullptr);
	~igQtCustomColorAreaWidget() override;

public:
	void setGivenColor(const QColor& c);
	void setGivenItemColor(int iIndex, const QColor& c);

signals:
	void sigColorItemSel(const QColor&);
protected:
	int  CurrentIndex;
	QGridLayout* LayoutGrid;
	QVector<igQtBasicColorItem*>mapIndexToItem;
};


#endif //IGQTCUSTOMCOLORAREAWIDGET_H