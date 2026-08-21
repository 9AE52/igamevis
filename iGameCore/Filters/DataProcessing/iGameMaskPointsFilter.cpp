#include "iGameMaskPointsFilter.h"
#include "iGameFlatArray.h"

#include <algorithm>
#include <random>
#include <vector>

IGAME_NAMESPACE_BEGIN

namespace
{

// Copy the selected elements from an input array while preserving
// the original array type, name, dimension, and value precision.
template<typename ArrayType>
ArrayObject::Pointer CopySelectedArray(const ArrayObject::Pointer& inputArray,
                                       const std::vector<igIndex>& selectedIds) {
    auto inArray = DynamicCast<ArrayType>(inputArray);

    if (inArray.IsNull()) { return nullptr; }

    auto outArray = ArrayType::New();

    outArray->SetName(inArray->GetName());

    const int dimension = inArray->GetDimension();

    outArray->SetDimension(dimension);
    outArray->Resize(static_cast<IGsize>(selectedIds.size()));

    for (IGsize newId = 0; newId < static_cast<IGsize>(selectedIds.size()); ++newId) {

        const igIndex oldId = selectedIds[static_cast<size_t>(newId)];

        const auto* src = inArray->RawPointer(oldId);

        auto* dst = outArray->RawPointer(newId);

        std::copy(src, src + dimension, dst);
    }

    return outArray;
}

} // namespace


MaskPointsFilter::MaskPointsFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}


void MaskPointsFilter::SetOnRatio(int ratio) { m_OnRatio = ratio; }


int MaskPointsFilter::GetOnRatio() const { return m_OnRatio; }


void MaskPointsFilter::SetOffset(IGsize offset) { m_Offset = offset; }


IGsize MaskPointsFilter::GetOffset() const { return m_Offset; }


void MaskPointsFilter::SetMaximumNumberOfPoints(IGsize maxPoints) { m_MaximumNumberOfPoints = maxPoints; }


IGsize MaskPointsFilter::GetMaximumNumberOfPoints() const { return m_MaximumNumberOfPoints; }


void MaskPointsFilter::SetRandomMode(bool enabled) { m_RandomMode = enabled; }


bool MaskPointsFilter::GetRandomMode() const { return m_RandomMode; }


void MaskPointsFilter::SetRandomSeed(unsigned int seed) { m_RandomSeed = seed; }


unsigned int MaskPointsFilter::GetRandomSeed() const { return m_RandomSeed; }


bool MaskPointsFilter::Execute() {
    // MaskPoints currently accepts an UnstructuredMesh as input.
    auto input = DynamicCast<UnstructuredMesh>(GetInput(0));

    if (input.IsNull()) { return false; }

    // OnRatio must be a positive sampling interval.
    if (m_OnRatio <= 0) { return false; }

    const IGsize numberOfPoints = input->GetNumberOfPoints();

    auto output = UnstructuredMesh::New();

    std::vector<igIndex> selectedIds;


    // ------------------------------------------------------------
    // Fixed-stride sampling
    //
    // Starting from Offset, select every OnRatio-th point.
    // MaximumNumberOfPoints = 0 means no additional limit.
    // ------------------------------------------------------------
    if (!m_RandomMode) {

        for (IGsize oldId = m_Offset; oldId < numberOfPoints; oldId += m_OnRatio) {

            selectedIds.push_back(static_cast<igIndex>(oldId));

            if (m_MaximumNumberOfPoints > 0 && selectedIds.size() >= static_cast<size_t>(m_MaximumNumberOfPoints)) {
                break;
            }
        }
    }


    // ------------------------------------------------------------
    // Random sampling
    //
    // Random sampling ignores OnRatio when determining which point
    // IDs are selected. The same RandomSeed produces reproducible
    // selections for the same input and runtime environment.
    // ------------------------------------------------------------
    else {

        if (m_Offset < numberOfPoints) {

            std::vector<igIndex> allIds;

            allIds.reserve(static_cast<size_t>(numberOfPoints - m_Offset));

            // Only points at or after Offset participate in sampling.
            for (IGsize oldId = m_Offset; oldId < numberOfPoints; ++oldId) {

                allIds.push_back(static_cast<igIndex>(oldId));
            }

            std::mt19937 generator(m_RandomSeed);

            std::shuffle(allIds.begin(), allIds.end(), generator);

            IGsize targetCount = static_cast<IGsize>(allIds.size());

            if (m_MaximumNumberOfPoints > 0 && targetCount > m_MaximumNumberOfPoints) {

                targetCount = m_MaximumNumberOfPoints;
            }

            selectedIds.assign(allIds.begin(), allIds.begin() + static_cast<size_t>(targetCount));
        }
    }


    // ------------------------------------------------------------
    // Create output geometry
    //
    // Each selected input point becomes a new output point.
    // A vertex cell is created for every output point so that the
    // resulting point set can be represented as an UnstructuredMesh.
    // ------------------------------------------------------------
    for (igIndex oldId: selectedIds) {

        const Point& point = input->GetPoint(oldId);

        IGsize newId = output->AddPoint(point);

        igIndex cell[1] = {static_cast<igIndex>(newId)};

        output->AddCell(cell, 1, IG_VERTEX);
    }

    output->SetName(input->GetName());


    // ------------------------------------------------------------
    // Copy point attributes
    //
    // Only attributes attached to points are preserved because the
    // output contains newly generated vertex cells rather than the
    // original cell topology.
    //
    // The original array type is preserved during copying.
    // ------------------------------------------------------------
    auto inData = input->GetAttributeSet();

    if (inData != nullptr) {

        auto outData = AttributeSet::New();

        auto inAllAttr = inData->GetAllAttributes();

        for (IGsize i = 0; i < inAllAttr->GetNumberOfElements(); ++i) {

            auto attr = inAllAttr->GetElement(i);

            if (attr.attachmentType != IG_POINT) { continue; }

            auto inArray = attr.pointer;

            ArrayObject::Pointer outArray;


            switch (inArray->GetArrayType()) {

                case IG_FloatArray:
                    outArray = CopySelectedArray<FloatArray>(inArray, selectedIds);
                    break;


                case IG_DoubleArray:
                    outArray = CopySelectedArray<DoubleArray>(inArray, selectedIds);
                    break;


                case IG_IntArray:
                    outArray = CopySelectedArray<IntArray>(inArray, selectedIds);
                    break;


                case IG_UnsignedIntArray:
                    outArray = CopySelectedArray<UnsignedIntArray>(inArray, selectedIds);
                    break;


                case IG_CharArray:
                    outArray = CopySelectedArray<CharArray>(inArray, selectedIds);
                    break;


                case IG_UnsignedCharArray:
                    outArray = CopySelectedArray<UnsignedCharArray>(inArray, selectedIds);
                    break;


                case IG_ShortArray:
                    outArray = CopySelectedArray<ShortArray>(inArray, selectedIds);
                    break;


                case IG_UnsignedShortArray:
                    outArray = CopySelectedArray<UnsignedShortArray>(inArray, selectedIds);
                    break;


                case IG_LongLongArray:
                    outArray = CopySelectedArray<LongLongArray>(inArray, selectedIds);
                    break;


                case IG_UnsignedLongLongArray:
                    outArray = CopySelectedArray<UnsignedLongLongArray>(inArray, selectedIds);
                    break;


                default:
                    // Unsupported array types are skipped instead of
                    // being silently converted to a different type.
                    continue;
            }


            if (outArray == nullptr) { continue; }


            outData->AddAttribute(attr.type, attr.attachmentType, outArray, attr.GetDataRange());
        }


        output->SetAttributeSet(outData);
    }


    SetOutput(output);

    return true;
}

IGAME_NAMESPACE_END