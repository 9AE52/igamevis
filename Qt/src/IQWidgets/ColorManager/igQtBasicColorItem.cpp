#include <IQWidgets/ColorManager/igQtBasicColorItem.h>
#include <QPixmap>

#define ITEM_EDGE_LENGTH 22
igQtBasicColorItem::igQtBasicColorItem(const QColor& c, QWidget* parent)
	: QLabel(parent)
	, myMouseIn(false)
{
	setFixedSize(ITEM_EDGE_LENGTH, ITEM_EDGE_LENGTH);
	myColor = c;
	this->update();
}
igQtBasicColorItem::~igQtBasicColorItem() {
}
// 设置颜色
void igQtBasicColorItem::setColor(const QColor& c)
{
	myColor = c;
	QImage itemImg = QImage(ITEM_EDGE_LENGTH, ITEM_EDGE_LENGTH, QImage::Format_RGB32);
	QPainter painter(&itemImg);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawImage(0, 0, itemImg);
	painter.setPen(Qt::NoPen);
	painter.setBrush(c);
	painter.drawRect(0, 0, width(), height());
	painter.end();
	this->setPixmap(QPixmap::fromImage(itemImg));
	update();
}

// 获取颜色
QColor igQtBasicColorItem::getColor()
{
	return myColor;
}

void igQtBasicColorItem::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	if (myMouseIn)
	    painter.setPen(QPen(Qt::white, 3));
	else
	    painter.setPen(QPen(myColor, 0));
	painter.setBrush(myColor);
	painter.drawRect(1, 1, ITEM_EDGE_LENGTH - 3, ITEM_EDGE_LENGTH - 3);

}

void igQtBasicColorItem::mousePressEvent(QMouseEvent* event)
{
	Q_UNUSED(event);

	Q_EMIT sigItemClicked(myColor);
}

void igQtBasicColorItem::enterEvent(QEvent* event)
{
	Q_UNUSED(event);

	myMouseIn = true;
	this->update();
}

void igQtBasicColorItem::leaveEvent(QEvent* event)
{
	Q_UNUSED(event);

	myMouseIn = false;
	this->update();
}