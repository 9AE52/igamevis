#include "iGameGenerateProcessIdsFilter.h"

IGAME_NAMESPACE_BEGIN

GenerateProcessIdsFilter::GenerateProcessIdsFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}

bool GenerateProcessIdsFilter::Execute() {
    auto input = GetInput(0);
    if (input == nullptr) return false;

    auto mesh = DynamicCast<PointSet>(input);
    if (mesh == nullptr) return false;

    if (m_GeneratePointData) {
        IntArray::Pointer ids = IntArray::New();
        ids->SetName("PointProcessIds");
        IGsize pointNum = mesh->GetNumberOfPoints();
        ids->Resize(pointNum);
        for (IGsize i = 0; i < pointNum; ++i) {
            ids->SetValue(i, m_ProcessId);
        }
        input->GetAttributeSet()->AddScalar(IG_POINT, ids);
    }

    if (m_GenerateCellData) {
        auto unstructuredMesh = DynamicCast<UnstructuredMesh>(input);
        if (unstructuredMesh != nullptr) {
            IntArray::Pointer ids = IntArray::New();
            ids->SetName("CellProcessIds");
            IGsize cellNum = unstructuredMesh->GetNumberOfCells();
            ids->Resize(cellNum);
            for (IGsize i = 0; i < cellNum; ++i) {
                ids->SetValue(i, m_ProcessId);
            }
            input->GetAttributeSet()->AddScalar(IG_CELL, ids);
        }
    }

    SetOutput(input);
    return true;
}

IGAME_NAMESPACE_END
