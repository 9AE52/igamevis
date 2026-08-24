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

    auto attributeSet = input->GetAttributeSet();
    if (attributeSet == nullptr) return false;

    m_PointProcessIdArray = nullptr;
    m_CellProcessIdArray = nullptr;

    if (m_GeneratePointData) {
        auto pointAttrs = attributeSet->GetAllPointAttributes();
        if (pointAttrs != nullptr) {
            for (int i = 0; i < pointAttrs->GetNumberOfElements(); ++i) {
                auto& attr = pointAttrs->GetElement(i);
                if (attr.pointer != nullptr && attr.pointer->GetName() == "process_id") {
                    m_PointProcessIdArray = DynamicCast<LongLongArray>(attr.pointer);
                    break;
                }
            }
        }
        LongLongArray::Pointer ids = nullptr;
        auto& attr = attributeSet->GetScalar("PointProcessIds");
        if (!attr.IsNone()) ids = DynamicCast<LongLongArray>(attr.pointer);
        if (ids == nullptr) {
            int idx = attributeSet->GetAttributeIndex("PointProcessIds");
            if (idx >= 0) attributeSet->DeleteAttribute(idx);
            ids = LongLongArray::New();
            ids->SetName("PointProcessIds");
            attributeSet->AddScalar(IG_POINT, ids);
        }
        IGsize pointNum = mesh->GetNumberOfPoints();
        ids->Resize(pointNum);
        for (IGsize i = 0; i < pointNum; ++i) {
            ids->SetValue(i, GetPointProcessId(i));
        }
    }

    if (m_GenerateCellData) {
        auto unstructuredMesh = DynamicCast<UnstructuredMesh>(input);
        if (unstructuredMesh != nullptr) {
            auto cellAttrs = attributeSet->GetAllCellAttributes();
            if (cellAttrs != nullptr) {
                for (int i = 0; i < cellAttrs->GetNumberOfElements(); ++i) {
                    auto& attr = cellAttrs->GetElement(i);
                    if (attr.pointer != nullptr && attr.pointer->GetName() == "process_id") {
                        m_CellProcessIdArray = DynamicCast<LongLongArray>(attr.pointer);
                        break;
                    }
                }
            }
            LongLongArray::Pointer ids = nullptr;
            auto& attr = attributeSet->GetScalar("CellProcessIds");
            if (!attr.IsNone()) ids = DynamicCast<LongLongArray>(attr.pointer);
            if (ids == nullptr) {
                int idx = attributeSet->GetAttributeIndex("CellProcessIds");
                if (idx >= 0) attributeSet->DeleteAttribute(idx);
                ids = LongLongArray::New();
                ids->SetName("CellProcessIds");
                attributeSet->AddScalar(IG_CELL, ids);
            }
            IGsize cellNum = unstructuredMesh->GetNumberOfCells();
            ids->Resize(cellNum);
            for (IGsize i = 0; i < cellNum; ++i) {
                ids->SetValue(i, GetCellProcessId(i));
            }
        }
    }

    SetOutput(input);
    return true;
}

long long GenerateProcessIdsFilter::GetPointProcessId(IGsize index) {
    if (m_PointProcessIdArray != nullptr && index < m_PointProcessIdArray->GetNumberOfElements()) {
        return static_cast<long long>(m_PointProcessIdArray->GetValue(index));
    }
    return m_ProcessId;
}

long long GenerateProcessIdsFilter::GetCellProcessId(IGsize index) {
    if (m_CellProcessIdArray != nullptr && index < m_CellProcessIdArray->GetNumberOfElements()) {
        return static_cast<long long>(m_CellProcessIdArray->GetValue(index));
    }
    return m_ProcessId;
}

IGAME_NAMESPACE_END
