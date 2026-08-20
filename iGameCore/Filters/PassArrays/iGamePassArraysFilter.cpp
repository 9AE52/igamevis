#include "iGamePassArraysFilter.h"
#include "iGameArrayObject.h"
#include "iGameAttributeSet.h"
#include "iGameCellArray.h"

#include "iGamePoints.h"
#include "iGameStructuredMesh.h"
#include "iGameSurfaceMesh.h"
#include "iGameUnstructuredMesh.h"
#include "iGameVolumeMesh.h"
#include <algorithm>

IGAME_NAMESPACE_BEGIN

// 辅助函数：深拷贝一个 ArrayObject（根据实际类型调用对应 DeepCopy）
static ArrayObject::Pointer DeepCopyArray(ArrayObject::Pointer src) {
    if (!src) return nullptr;

    // 转换为具体类型并调用 DeepCopy
    if (auto farr = DynamicCast<FloatArray>(src)) {
        auto copy = FloatArray::New();
        copy->DeepCopy(farr);
        copy->SetName(src->GetName());
        return copy;
    }
    if (auto darr = DynamicCast<DoubleArray>(src)) {
        auto copy = DoubleArray::New();
        copy->DeepCopy(darr);
        copy->SetName(src->GetName());
        return copy;
    }
    if (auto uarr = DynamicCast<UnsignedIntArray>(src)) {
        auto copy = UnsignedIntArray::New();
        copy->DeepCopy(uarr);
        copy->SetName(src->GetName());
        return copy;
    }

    // 如果都不匹配，退回浅拷贝
    return src;
}

bool PassArrays::Execute() {
    DataObject::Pointer input = GetInput(0);
    if (!input) return false;

    IGenum type = input->GetDataObjectType();
    // 属性深拷贝过滤
    auto inAttrSet = input->GetAttributeSet();
    auto outAttrSet = AttributeSet::New();
    if (inAttrSet) {
        // 点属性
        auto pointAttrs = inAttrSet->GetAllPointAttributes();
        if (pointAttrs) {
            for (IGsize i = 0; i < pointAttrs->GetNumberOfElements(); ++i) {
                auto& attr = pointAttrs->GetElement(i);
                if (attr.IsNone()) continue;
                if (std::find(m_ArrayNames.begin(), m_ArrayNames.end(), attr.pointer->GetName()) !=
                    m_ArrayNames.end()) {
                    // 深拷贝属性数组
                    auto copiedArray = DeepCopyArray(attr.pointer);
                    if (copiedArray) {

                        DoubleArray::Pointer copiedRange = nullptr;
                        if (attr.dataRange) {
                            copiedRange = DoubleArray::New();
                            copiedRange->DeepCopy(attr.dataRange);
                        }
                        outAttrSet->AddAttribute(attr.type, IG_POINT, copiedArray, copiedRange);
                    }
                }
            }
        }
        // 单元属性
        auto cellAttrs = inAttrSet->GetAllCellAttributes();
        if (cellAttrs) {
            for (IGsize i = 0; i < cellAttrs->GetNumberOfElements(); ++i) {
                auto& attr = cellAttrs->GetElement(i);
                if (attr.IsNone()) continue;
                if (std::find(m_ArrayNames.begin(), m_ArrayNames.end(), attr.pointer->GetName()) !=
                    m_ArrayNames.end()) {
                    auto copiedArray = DeepCopyArray(attr.pointer);
                    if (copiedArray) {
                        DoubleArray::Pointer copiedRange = nullptr;
                        if (attr.dataRange) {
                            copiedRange = DoubleArray::New();
                            copiedRange->DeepCopy(attr.dataRange);
                        }
                        outAttrSet->AddAttribute(attr.type, IG_CELL, copiedArray, copiedRange);
                    }
                }
            }
        }
    }
    input->SetAttributeSet(outAttrSet);


    SetOutput(0, input);
    return true;
}

IGAME_NAMESPACE_END