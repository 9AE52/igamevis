#include "iGamePointAndCellIdsFilter.h"

#include "iGameAttributeSet.h"
#include "iGameFlatArray.h"

IGAME_NAMESPACE_BEGIN

namespace
{
void AddOrReplaceScalar(AttributeSet* attributes, IGenum attachmentType, ArrayObject::Pointer array) {
    auto allAttributes = attributes->GetAllAttributes();

    for (IGsize i = 0; i < allAttributes->GetNumberOfElements(); ++i) {

        auto& attr = allAttributes->GetElement(i);

        if (!attr.isDeleted && attr.pointer != nullptr && attr.attachmentType == attachmentType &&
            attr.pointer->GetName() == array->GetName()) {

            attr.pointer = array;
            attr.type = IG_SCALAR;

            attr.dataRange = nullptr;

            return;
        }
    }

    attributes->AddScalar(attachmentType, array);
}

}


PointAndCellIdsFilter::PointAndCellIdsFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}


bool PointAndCellIdsFilter::Execute() {
    auto input = GetInput(0);

    if (input == nullptr) { return false; }

    auto attributes = input->GetAttributeSet();

    if (attributes == nullptr) { return false; }

    auto points = input->GetPoints();
    auto cells = input->GetCellArray();

    if (m_GeneratePointIds && points == nullptr) { return false; }

    if (m_GenerateCellIds && cells == nullptr) { return false; }

    if (m_GeneratePointIds) {

        const IGsize pointCount = points->GetNumberOfPoints();

        auto pointIds = IntArray::New();

        pointIds->SetName(m_PointIdsArrayName);
        pointIds->SetDimension(1);
        pointIds->Resize(pointCount);

        for (IGsize i = 0; i < pointCount; ++i) { pointIds->SetValue(i, static_cast<double>(i)); }

        AddOrReplaceScalar(attributes, IG_POINT, pointIds);
    }

    if (m_GenerateCellIds) {

        const IGsize cellCount = cells->GetNumberOfCells();

        auto cellIds = IntArray::New();

        cellIds->SetName(m_CellIdsArrayName);
        cellIds->SetDimension(1);
        cellIds->Resize(cellCount);

        for (IGsize i = 0; i < cellCount; ++i) { cellIds->SetValue(i, static_cast<double>(i)); }

        AddOrReplaceScalar(attributes, IG_CELL, cellIds);
    }

    attributes->ForceReConvertToDrawableData();

    SetOutput(input);

    UpdateProgress(1.0);

    return true;
}

IGAME_NAMESPACE_END