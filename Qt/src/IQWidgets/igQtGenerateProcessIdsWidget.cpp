#include "IQWidgets/igQtGenerateProcessIdsWidget.h"
#include "iGameFilterIncludes.h"

igQtGenerateProcessIdsWidget::igQtGenerateProcessIdsWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GenerateProcessIdsWidget) {
    ui->setupUi(this);
    connect(ui->btnApply, &QPushButton::clicked, this, &igQtGenerateProcessIdsWidget::Apply);
}

void igQtGenerateProcessIdsWidget::SetOriginDataObject(iGame::DataObject::Pointer data) {
    m_OriginDataObject = data;
}

void igQtGenerateProcessIdsWidget::Apply() {
    if (m_OriginDataObject == nullptr) return;
    auto filter = iGame::GenerateProcessIdsFilter::New();
    filter->SetInput(m_OriginDataObject);
    filter->SetGeneratePointData(ui->checkBox_PointData->isChecked());
    filter->SetGenerateCellData(ui->checkBox_CellData->isChecked());
    if (filter->Execute()) {
        Q_EMIT UpdateProcessIdsModel(m_OriginDataObject);
    }
}
