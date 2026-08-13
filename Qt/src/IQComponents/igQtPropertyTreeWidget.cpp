//#include <IQComponents/igQtPropertyTreeWidget.h>
//#include <QLineEdit>
//#include <QSpinBox>
//#include <QComboBox>
//#include <QCheckBox>
//
//igQtPropertyTreeWidget::igQtPropertyTreeWidget(QWidget* parent) : QTreeWidget(parent)
//{
//    connect(this, &QTreeWidget::itemClicked, this, &igQtPropertyTreeWidget::onItemClicked);
//
//
//    setIndentation(0);
//
//    this->setStyleSheet(
//        "QTreeWidget {"
//        "    border: 1px solid black;" 
//        "    border-collapse: collapse;" 
//        "}"
//        "QTreeWidget::item {"
//        "    border-bottom: 1px solid black;" 
//        "}"
//        "QHeaderView::section {"
//        "    border: 1px solid black;" 
//        "}"
//    );
//}
//
//QTreeWidgetItem* igQtPropertyTreeWidget::addCategory(const QString& name) {
//    QTreeWidgetItem* category = new QTreeWidgetItem(this);
//    category->setText(0, name);
//    return category;
//}
//
//void igQtPropertyTreeWidget::addTextProperty(QTreeWidgetItem* category, const QString& name, const QString& defaultValue) {
//    //PropertyTreeWidgetItem* propertyItem = new PropertyTreeWidgetItem(category);
//    //propertyItem->setNameText(name);
//    //PropertyLineEdit* lineEdit = new PropertyLineEdit;
//    //lineEdit->setText(defaultValue);
//    //this->setItemWidget(propertyItem, 1, lineEdit);
//    //lineEdit->hide();
//    //lineEdit->setFocusInFunctor(&igQtPropertyTreeWidget::hideWidget, this, propertyItem, lineEdit);
//    //propertyItem->setValueWidget(lineEdit, PropertyTreeWidgetItem::LineEdit);
//    //propertyItem->setValueText(defaultValue);
//
//
//    QTreeWidgetItem* propertyItem = new QTreeWidgetItem(category);
//    propertyItem->setText(0, name);
//    EditTextWidget* editTextWidget = new EditTextWidget();
//    editTextWidget->setText(defaultValue);
//    editTextWidget->resize(QSize(100, 20));
//    this->setItemWidget(propertyItem, 1, editTextWidget);
//
//    
//}
//
//void igQtPropertyTreeWidget::addIntegerProperty(QTreeWidgetItem* category, const QString& name, int defaultValue) {
//    //PropertyTreeWidgetItem* propertyItem = new PropertyTreeWidgetItem(category);
//    //propertyItem->setNameText(name);
//    //QSpinBox* spinBox = new QSpinBox();
//    //spinBox->setValue(defaultValue);
//    //propertyItem->setValueWidget(spinBox, PropertyTreeWidgetItem::SpinBox);
//    //propertyItem->setValueText(QString::number(defaultValue));
//}
//
//void igQtPropertyTreeWidget::addEnumProperty(QTreeWidgetItem* category, const QString& name, const QStringList& options, const QString& defaultValue) {
//    //PropertyTreeWidgetItem* propertyItem = new PropertyTreeWidgetItem(category);
//    //propertyItem->setNameText(name);
//    //QComboBox* comboBox = new QComboBox();
//    //comboBox->addItems(options);
//    //comboBox->setCurrentText(defaultValue);
//    //propertyItem->setValueWidget(comboBox, PropertyTreeWidgetItem::ComboBox);
//    //propertyItem->setValueText(defaultValue);
//}