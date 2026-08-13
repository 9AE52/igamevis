#include "IQWidgets/ColorManager/igQtPreviewColorAreaWidget.h"

igQtPreviewColorAreaWidget::igQtPreviewColorAreaWidget(QWidget* parent)
	: QWidget(parent)
	, oldColor(Qt::white)
	, newColor(Qt::black)
{
	// 初始化界面
	this->setFixedSize(166, 80);
	this->setWindowFlag(Qt::FramelessWindowHint);
}
igQtPreviewColorAreaWidget::~igQtPreviewColorAreaWidget() {

}
// 设置当前颜色
void igQtPreviewColorAreaWidget::setOldColor(const QColor& c)
{
	oldColor = c;
	repaint();
}

// 设置新的颜色
void igQtPreviewColorAreaWidget::setNewColor(const QColor& c)
{
	newColor = c;
	update();
}

void igQtPreviewColorAreaWidget::paintEvent(QPaintEvent*)
{
	QStylePainter painter(this);
	paint(painter, geometry());
}

void igQtPreviewColorAreaWidget::resizeEvent(QResizeEvent*)
{
	update();
}

void igQtPreviewColorAreaWidget::paint(QPainter& painter, QRect rect) const
{
	int iMiddleWidth = rect.width() / 2;
	int iHeight = rect.height();
	painter.fillRect(0, 0, iMiddleWidth, iHeight, oldColor);
	painter.fillRect(iMiddleWidth, 0, iMiddleWidth, iHeight, newColor);
	//painter.setPen(QPen(Qt::black, 1));
	//painter.drawRect(0, 0, width() - 1, height() - 1);
}

void igQtPreviewColorAreaWidget::slotSvChanged(QColor c)
{
	newColor = c;
	update();
	Q_EMIT sigSvChanged(newColor);
}