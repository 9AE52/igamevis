#ifndef IGQTBASICCOLORAREAWIDGET_H
#define IGQTBASICCOLORAREAWIDGET_H

#include<QWidget>
#include <IQCore/igQtExportModule.h>
#include <IQWidgets/ColorManager/igQtBasicColorItem.h>
#include <QGridLayout>
class IG_QT_MODULE_EXPORT  igQtBasicColorAreaWidget : public QWidget {
	Q_OBJECT

public:
	igQtBasicColorAreaWidget(QWidget* parent = nullptr);
	~igQtBasicColorAreaWidget() override;

signals:
	void sigColorItemSel(QColor );
protected:
	QGridLayout* LayoutGrid{ nullptr };
	igQtBasicColorItem* BasicColorItem{ nullptr };
};


#endif //IGQTBASICCOLORAREAWIDGET_H