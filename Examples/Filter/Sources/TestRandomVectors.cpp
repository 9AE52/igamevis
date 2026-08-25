#include <Sources/iGameRandomVectorsFilter.h>
#include <iGameAttributeSet.h>
#include <iGameFlatArray.h>
#include <iGamePointSet.h>
#include <iGameType.h>
#include <iGameUnstructuredMesh.h>

#include <cmath>
#include <iostream>
#include <string>

namespace {

bool CheckVectorAttribute(const iGame::DataObject::Pointer& obj, unsigned int expectedPoints,
                          unsigned int expectedDimension, float minValue, float maxValue, std::string& reason) {
    auto mesh = iGame::DynamicCast<iGame::UnstructuredMesh>(obj);
    if (!mesh) {
        reason = "output is not an UnstructuredMesh";
        return false;
    }

    if (mesh->GetNumberOfPoints() != expectedPoints) {
        reason = "point count mismatch: expected " + std::to_string(expectedPoints) + ", got " +
                 std::to_string(mesh->GetNumberOfPoints());
        return false;
    }

    auto attrSet = mesh->GetAttributeSet();
    auto& attr = attrSet->GetVector("random_vectors");
    if (attr.pointer == nullptr) {
        reason = "vector attribute 'random_vectors' not found";
        return false;
    }

    if (attr.attachmentType != IG_POINT) {
        reason = "vector attribute is not attached to points";
        return false;
    }

    auto vectors = iGame::DynamicCast<iGame::FloatArray>(attr.pointer);
    if (!vectors) {
        reason = "vector attribute is not a FloatArray";
        return false;
    }

    if (static_cast<unsigned int>(vectors->GetDimension()) != expectedDimension) {
        reason = "vector dimension mismatch";
        return false;
    }

    if (static_cast<unsigned int>(vectors->GetNumberOfElements()) != expectedPoints) {
        reason = "vector element count mismatch";
        return false;
    }

    for (IGsize i = 0; i < vectors->GetNumberOfValues(); ++i) {
        const float v = vectors->ValueAt(i);
        if (v < minValue || v > maxValue) {
            reason = "value " + std::to_string(v) + " out of range [" + std::to_string(minValue) + ", " +
                     std::to_string(maxValue) + "]";
            return false;
        }
    }
    return true;
}

} // namespace

int main() {
    const unsigned int pointCount = 10000;
    const unsigned int dimension = 3;
    const float minValue = -1.f;
    const float maxValue = 1.f;
    const unsigned int seed = 42;

    auto filter = iGame::RandomVectorsFilter::New();
    filter->SetNumberOfPoints(pointCount);
    filter->SetVectorDimension(dimension);
    filter->SetRange(minValue, maxValue);
    filter->SetSeed(seed);

    if (!filter->Execute()) {
        std::cerr << "Result: FAIL\n";
        std::cerr << "Filter Execute failed\n";
        return 1;
    }

    auto output = filter->GetOutput();
    std::string reason;
    if (!CheckVectorAttribute(output, pointCount, dimension, minValue, maxValue, reason)) {
        std::cerr << "Result: FAIL\n";
        std::cerr << reason << "\n";
        return 1;
    }

    auto mesh = iGame::DynamicCast<iGame::UnstructuredMesh>(output);
    auto vectors = iGame::DynamicCast<iGame::FloatArray>(
            output->GetAttributeSet()->GetVector("random_vectors").pointer);
    std::cout << "Points: " << mesh->GetNumberOfPoints() << "\n";
    std::cout << "Dimension: " << vectors->GetDimension() << "\n";
    std::cout << "Elements: " << vectors->GetNumberOfElements() << "\n";
    std::cout << "Values: " << vectors->GetNumberOfValues() << "\n";
    std::cout << "Sample[0]: " << vectors->ValueAt(0) << " " << vectors->ValueAt(1) << " " << vectors->ValueAt(2)
              << "\n";
    std::cout << "Range: [" << minValue << ", " << maxValue << "]\n";

    auto filter2 = iGame::RandomVectorsFilter::New();
    filter2->SetNumberOfPoints(pointCount);
    filter2->SetVectorDimension(dimension);
    filter2->SetRange(minValue, maxValue);
    filter2->SetSeed(seed);
    filter2->Execute();
    auto output2 = filter2->GetOutput();
    auto vectors2 = iGame::DynamicCast<iGame::FloatArray>(
            output2->GetAttributeSet()->GetVector("random_vectors").pointer);
    bool reproducible = vectors->GetNumberOfValues() == vectors2->GetNumberOfValues();
    if (reproducible) {
        for (IGsize i = 0; i < vectors->GetNumberOfValues(); ++i) {
            if (vectors->ValueAt(i) != vectors2->ValueAt(i)) {
                reproducible = false;
                break;
            }
        }
    }
    std::cout << "Same seed reproducible: " << (reproducible ? "yes" : "no") << "\n";

    if (!reproducible) {
        std::cerr << "Result: FAIL\n";
        std::cerr << "same seed produced different random values\n";
        return 1;
    }

    std::cout << "Result: PASS\n";
    return 0;
}
