#include "IQWidgets/ColorManager/igQtHColorAreaWidget.h"


igQtHColorAreaWidget::igQtHColorAreaWidget(QWidget* parent)
	: QWidget(parent)
	, doubleHue(0.0)
	, intHue(0)
	, colorHeight(256)
	, colorWidth(25)
{
	this->setFixedSize(34, 270);

	// 创建色调pixmap
	createHuePixmap();
}
igQtHColorAreaWidget::~igQtHColorAreaWidget() {

}
void igQtHColorAreaWidget::setHue(int h)
{
	doubleHue = (double)h / 360;
	intHue = h;
	update();
}

void igQtHColorAreaWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.drawPixmap(0, 0, huePixmap);

	int iHeight = colorHeight - doubleHue * colorHeight;
	QPolygonF triangle;
	triangle.append(QPointF(colorWidth, iHeight + topMargin));
	triangle.append(QPointF(width(), iHeight));
	triangle.append(QPointF(width(), iHeight + 2 * topMargin - 1));
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawPolygon(triangle);
}

void igQtHColorAreaWidget::mousePressEvent(QMouseEvent* ev)
{
	double tempValue = 1.0 - (ev->pos().y() - topMargin) * 1.0 / (1.0 * colorHeight);
	doubleHue = qBound(0.0, tempValue, 1.0);
	update();
	intHue = doubleHue * 360;
	Q_EMIT sigHueChanged(intHue);
}

void igQtHColorAreaWidget::mouseMoveEvent(QMouseEvent* ev)
{
	double tempValue = 1 - (ev->pos().y() - topMargin) * 1.0 / (1.0 * colorHeight);
	doubleHue = qBound(0.0, tempValue, 1.0);
	update();
	intHue = doubleHue * 360;
	Q_EMIT sigHueChanged(intHue);
}

// 创建色调pixmap
void igQtHColorAreaWidget::createHuePixmap()
{
	huePixmap = QPixmap(34, 270);
	huePixmap.fill(Qt::transparent);
	QPainter painter(&huePixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	QLinearGradient hueGradient(0, colorHeight, 0, 0);
	for (double i = 0; i < 1.0; i += 1.0 / 16)
	{
		hueGradient.setColorAt(i, QColor::fromHsvF(i, 1, 1, 1));
	}
	hueGradient.setColorAt(1, QColor::fromHsvF(0, 1, 1, 1));
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(hueGradient));
	painter.drawRect(0, topMargin, colorWidth, colorHeight);
}