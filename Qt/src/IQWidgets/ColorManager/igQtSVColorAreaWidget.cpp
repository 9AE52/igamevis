#include "IQWidgets/ColorManager/igQtSVColorAreaWidget.h"


igQtSVColorAreaWidget::igQtSVColorAreaWidget(QWidget* parent)
	: QWidget(parent)
	, myHue(0)
	, mySaturation(0)
	, myBrightness(0)
	, myAreaWidth(256)
	, myColor(Qt::blue)
{
	// 初始化界面
	//this->setFixedSize(256, 256);

	// 创建正方形区域的pixmap
	createSVPixmap();
	// 创建亮度渐变的pixmap
	createVPixmap();
	// 更新正方形区域的pixmap
	updateSVPixmap();
}
igQtSVColorAreaWidget::~igQtSVColorAreaWidget()
{

}

void igQtSVColorAreaWidget::setHue(int h)
{
	myHue = h;
	updateSVPixmap();
	update();
}

void igQtSVColorAreaWidget::setSaturation(int s)
{
	mySaturation = s;
	update();
}

void igQtSVColorAreaWidget::setBrightness(int v)
{
	myBrightness = v;
	update();
}

void igQtSVColorAreaWidget::setHsv(int h, int s, int v)
{
	myHue = h;
	mySaturation = s;
	myBrightness = v;
	updateSVPixmap();
	update();
}

void igQtSVColorAreaWidget::setColor(const QColor& c)
{
	myHue = c.hue();
	mySaturation = c.saturation();
	myBrightness = c.value();
	updateSVPixmap();
	myColor.setHsv(myHue, mySaturation, myBrightness);
	Q_EMIT sigSvChanged(myColor);
	update();
}

void igQtSVColorAreaWidget::paintEvent(QPaintEvent*)
{
	// 画正方形的颜色区域
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.drawPixmap(0, 0, mySVPixmap);
	painter.drawPixmap(0, 0, myVPixmap);

	// 画颜色选取点的圆圈（亮度>128为黑色；<128为白色；避免黑色圆圈可能在黑色背景下显示不清晰的问题）
	painter.setPen(QPen(myBrightness > 128 ? Qt::black : Qt::white, 2));
	QPointF selecPos = QPointF(mySaturation, 255 - myBrightness);
	painter.drawEllipse(selecPos, 6, 6);
}

void igQtSVColorAreaWidget::mousePressEvent(QMouseEvent* ev)
{
	mySaturation = qBound(0, ev->x(), 255);
	myBrightness = qBound(0, 255 - ev->y(), 255);
	myColor.setHsv(myHue, mySaturation, myBrightness);
	Q_EMIT sigSvChanged(myColor);
	update();
}

void igQtSVColorAreaWidget::mouseMoveEvent(QMouseEvent* ev)
{
	mySaturation = qBound(0, ev->x(), 255);
	myBrightness = qBound(0, 255 - ev->y(), 255);
	myColor.setHsv(myHue, mySaturation, myBrightness);
	Q_EMIT sigSvChanged(myColor);
	update();
}

// 创建亮度渐变的pixmap
void igQtSVColorAreaWidget::createVPixmap()
{
	myVPixmap = QPixmap(myAreaWidth, myAreaWidth);
	myVPixmap.fill(Qt::transparent);
	QPainter painter(&myVPixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	QLinearGradient vGradient(0, 0, 0, myAreaWidth);
	vGradient.setColorAt(0, QColor(0, 0, 0, 0));
	vGradient.setColorAt(1, QColor(0, 0, 0, 255));
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(vGradient));
	painter.drawRect(0, 0, myAreaWidth, myAreaWidth);
}

// 创建正方形区域的pixmap
void igQtSVColorAreaWidget::createSVPixmap()
{
	mySVPixmap = QPixmap(myAreaWidth, myAreaWidth);
	mySVPixmap.fill(Qt::transparent);
}

// 更新正方形区域的pixmap
void igQtSVColorAreaWidget::updateSVPixmap()
{
	QColor newColor;
	newColor.setHsv(myHue, 255, 255);
	QPainter painter(&mySVPixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	QLinearGradient svGradient(0, 0, myAreaWidth, 0);
	svGradient.setColorAt(1, newColor);
	svGradient.setColorAt(0, QColor("#ffffff"));
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(svGradient));
	painter.drawRect(0, 0, myAreaWidth, myAreaWidth);
}

// 色调改变的槽函数
void igQtSVColorAreaWidget::slotHueChanged(int h)
{
	myHue = h;
	updateSVPixmap();
	myColor.setHsv(myHue, mySaturation, myBrightness);
	Q_EMIT sigSvChanged(myColor);
	update();
}