#include <DataProcessing/iGameMaskPointsFilter.h>
#include <iGameAttributeSet.h>
#include <iGameFlatArray.h>
#include <iGameUnstructuredMesh.h>

#include <iostream>
#include <vector>


// ------------------------------------------------------------
// Create a test mesh containing 10 points.
//
// Point ID:      0 1 2 ... 9
// X coordinate:  0 1 2 ... 9
//
// Each point also carries a scalar attribute:
// TestScalar = 100 + pointId
// ------------------------------------------------------------
iGame::UnstructuredMesh::Pointer CreateTestMesh() {
    auto mesh = iGame::UnstructuredMesh::New();

    for (int i = 0; i < 10; ++i) { mesh->AddPoint(iGame::Point(static_cast<float>(i), 0.0f, 0.0f)); }

    auto scalarArray = iGame::FloatArray::New();

    scalarArray->SetName("TestScalar");
    scalarArray->SetDimension(1);
    scalarArray->Resize(10);

    for (int i = 0; i < 10; ++i) {
        double value[1] = {100.0 + static_cast<double>(i)};

        scalarArray->SetElement(i, value);
    }

    mesh->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_POINT, scalarArray);

    return mesh;
}


// ------------------------------------------------------------
// Check whether the output point coordinates match the expected
// x-coordinate sequence.
// ------------------------------------------------------------
bool CheckCoordinates(iGame::UnstructuredMesh::Pointer output, const std::vector<double>& expected) {
    if (output.IsNull()) { return false; }

    if (output->GetNumberOfPoints() != expected.size()) { return false; }

    for (IGsize i = 0; i < output->GetNumberOfPoints(); ++i) {

        const auto& p = output->GetPoint(i);

        if (p[0] != expected[static_cast<size_t>(i)]) { return false; }
    }

    return true;
}


// ------------------------------------------------------------
// Check whether the output TestScalar values match the expected
// point attribute values.
// ------------------------------------------------------------
bool CheckAttributes(iGame::UnstructuredMesh::Pointer output, const std::vector<double>& expected) {
    if (output.IsNull()) { return false; }

    auto attrSet = output->GetAttributeSet();

    if (attrSet == nullptr) { return false; }

    auto allAttrs = attrSet->GetAllAttributes();

    if (allAttrs->GetNumberOfElements() == 0) { return false; }

    for (IGsize i = 0; i < allAttrs->GetNumberOfElements(); ++i) {

        auto attr = allAttrs->GetElement(i);

        if (attr.attachmentType != IG_POINT) { continue; }

        auto array = attr.pointer;

        if (array->GetName() != "TestScalar") { continue; }

        if (array->GetNumberOfElements() != expected.size()) { return false; }

        double value[IGAME_CELL_MAX_SIZE] = {0};

        for (IGsize j = 0; j < static_cast<IGsize>(expected.size()); ++j) {

            array->GetElement(j, value);

            if (value[0] != expected[static_cast<size_t>(j)]) { return false; }
        }

        return true;
    }

    return false;
}


// ------------------------------------------------------------
// Test 1: OnRatio
//
// Input point IDs:
// 0 1 2 3 4 5 6 7 8 9
//
// OnRatio = 2
//
// Expected selected point IDs:
// 0 2 4 6 8
// ------------------------------------------------------------
bool TestOnRatio() {
    auto input = CreateTestMesh();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(2);
    filter->SetOffset(0);
    filter->SetMaximumNumberOfPoints(0);
    filter->SetRandomMode(false);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    return CheckCoordinates(output, {0, 2, 4, 6, 8});
}


// ------------------------------------------------------------
// Test 2: MaximumNumberOfPoints
//
// OnRatio = 2
// MaximumNumberOfPoints = 3
//
// Expected selected point IDs:
// 0 2 4
// ------------------------------------------------------------
bool TestMaximumNumberOfPoints() {
    auto input = CreateTestMesh();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(2);
    filter->SetOffset(0);
    filter->SetMaximumNumberOfPoints(3);
    filter->SetRandomMode(false);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    return CheckCoordinates(output, {0, 2, 4});
}


// ------------------------------------------------------------
// Test 3: Offset
//
// OnRatio = 3
// Offset = 1
//
// Expected selected point IDs:
// 1 4 7
// ------------------------------------------------------------
bool TestOffset() {
    auto input = CreateTestMesh();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(3);
    filter->SetOffset(1);
    filter->SetMaximumNumberOfPoints(0);
    filter->SetRandomMode(false);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    return CheckCoordinates(output, {1, 4, 7});
}


// ------------------------------------------------------------
// Test 4: RandomMode and RandomSeed
//
// Offset = 2
// MaximumNumberOfPoints = 3
// RandomSeed = 42
//
// Verify that:
// 1. Exactly three points are selected.
// 2. Two runs with the same seed produce identical results.
// 3. Every selected point satisfies the Offset constraint.
// ------------------------------------------------------------
bool TestRandomReproducibility() {
    auto input = CreateTestMesh();

    auto filter1 = iGame::MaskPointsFilter::New();

    filter1->SetInput(0, input);
    filter1->SetOnRatio(3);
    filter1->SetOffset(2);
    filter1->SetMaximumNumberOfPoints(3);
    filter1->SetRandomMode(true);
    filter1->SetRandomSeed(42);

    if (!filter1->Execute()) { return false; }

    auto output1 = iGame::DynamicCast<iGame::UnstructuredMesh>(filter1->GetOutput());

    auto filter2 = iGame::MaskPointsFilter::New();

    filter2->SetInput(0, input);
    filter2->SetOnRatio(3);
    filter2->SetOffset(2);
    filter2->SetMaximumNumberOfPoints(3);
    filter2->SetRandomMode(true);
    filter2->SetRandomSeed(42);

    if (!filter2->Execute()) { return false; }

    auto output2 = iGame::DynamicCast<iGame::UnstructuredMesh>(filter2->GetOutput());

    if (output1.IsNull() || output2.IsNull()) { return false; }

    if (output1->GetNumberOfPoints() != 3 || output2->GetNumberOfPoints() != 3) { return false; }

    for (IGsize i = 0; i < output1->GetNumberOfPoints(); ++i) {

        const auto& p1 = output1->GetPoint(i);
        const auto& p2 = output2->GetPoint(i);

        if (p1[0] < 2.0) { return false; }

        if (p1[0] != p2[0] || p1[1] != p2[1] || p1[2] != p2[2]) { return false; }
    }

    return true;
}


// ------------------------------------------------------------
// Test 5: Point Attributes
//
// For the test mesh:
// x = pointId
// TestScalar = 100 + pointId
//
// Verify that every selected output point keeps the scalar
// attribute associated with its original input point.
// Therefore:
// TestScalar = 100 + output point's x coordinate.
// ------------------------------------------------------------
bool TestPointAttributes() {
    auto input = CreateTestMesh();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(3);
    filter->SetOffset(2);
    filter->SetMaximumNumberOfPoints(3);
    filter->SetRandomMode(true);
    filter->SetRandomSeed(42);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    if (output.IsNull()) { return false; }

    if (output->GetNumberOfPoints() != 3) { return false; }

    auto attrSet = output->GetAttributeSet();

    if (attrSet == nullptr) { return false; }

    auto allAttrs = attrSet->GetAllAttributes();

    for (IGsize i = 0; i < allAttrs->GetNumberOfElements(); ++i) {

        auto attr = allAttrs->GetElement(i);

        if (attr.attachmentType != IG_POINT) { continue; }

        auto array = attr.pointer;

        if (array->GetName() != "TestScalar") { continue; }

        double value[IGAME_CELL_MAX_SIZE] = {0};

        for (IGsize j = 0; j < output->GetNumberOfPoints(); ++j) {

            const auto& point = output->GetPoint(j);

            array->GetElement(j, value);

            // In the input mesh:
            // x = pointId
            // TestScalar = 100 + pointId
            //
            // Therefore every selected output point must satisfy:
            // TestScalar = 100 + x
            const double expectedValue = 100.0 + static_cast<double>(point[0]);

            if (value[0] != expectedValue) { return false; }
        }

        return true;
    }

    return false;
}


// ------------------------------------------------------------
// Test 6: Attribute Type Preservation
//
// Create both DoubleArray and IntArray point attributes and
// verify that MaskPoints preserves:
// 1. The original array type.
// 2. The attribute values associated with the selected points.
// ------------------------------------------------------------
bool TestAttributeTypes() {
    auto input = iGame::UnstructuredMesh::New();

    for (int i = 0; i < 6; ++i) { input->AddPoint(iGame::Point(static_cast<float>(i), 0.0f, 0.0f)); }

    auto doubleArray = iGame::DoubleArray::New();

    doubleArray->SetName("DoubleScalar");
    doubleArray->SetDimension(1);
    doubleArray->Resize(6);

    auto intArray = iGame::IntArray::New();

    intArray->SetName("IntScalar");
    intArray->SetDimension(1);
    intArray->Resize(6);

    for (int i = 0; i < 6; ++i) {
        double d[1] = {1000.5 + static_cast<double>(i)};

        int v[1] = {200 + i};

        doubleArray->SetElement(i, d);
        intArray->SetElement(i, v);
    }

    input->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_POINT, doubleArray);

    input->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_POINT, intArray);

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(2);
    filter->SetOffset(0);
    filter->SetMaximumNumberOfPoints(0);
    filter->SetRandomMode(false);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    if (output.IsNull()) { return false; }

    if (output->GetNumberOfPoints() != 3) { return false; }

    auto attrSet = output->GetAttributeSet();

    if (attrSet == nullptr) { return false; }

    auto allAttrs = attrSet->GetAllAttributes();

    bool foundDouble = false;
    bool foundInt = false;

    for (IGsize i = 0; i < allAttrs->GetNumberOfElements(); ++i) {

        auto attr = allAttrs->GetElement(i);

        if (attr.attachmentType != IG_POINT) { continue; }

        auto array = attr.pointer;

        if (array->GetName() == "DoubleScalar") {

            if (array->GetArrayType() != IG_DoubleArray) { return false; }

            double value[1] = {0.0};

            array->GetElement(0, value);
            if (value[0] != 1000.5) { return false; }

            array->GetElement(1, value);
            if (value[0] != 1002.5) { return false; }

            array->GetElement(2, value);
            if (value[0] != 1004.5) { return false; }

            foundDouble = true;
        }

        if (array->GetName() == "IntScalar") {

            if (array->GetArrayType() != IG_IntArray) { return false; }

            int value[1] = {0};

            array->GetElement(0, value);
            if (value[0] != 200) { return false; }

            array->GetElement(1, value);
            if (value[0] != 202) { return false; }

            array->GetElement(2, value);
            if (value[0] != 204) { return false; }

            foundInt = true;
        }
    }

    return foundDouble && foundInt;
}


// ------------------------------------------------------------
// Test 7: Empty Input
//
// Verify that an empty input mesh is handled successfully and
// produces an empty output mesh.
// ------------------------------------------------------------
bool TestEmptyInput() {
    auto input = iGame::UnstructuredMesh::New();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(2);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    if (output.IsNull()) { return false; }

    return output->GetNumberOfPoints() == 0;
}


// ------------------------------------------------------------
// Test 8: Offset Out of Range
//
// Verify that an Offset beyond the available point range is
// handled successfully and produces an empty output mesh.
// ------------------------------------------------------------
bool TestOffsetOutOfRange() {
    auto input = CreateTestMesh();

    auto filter = iGame::MaskPointsFilter::New();

    filter->SetInput(0, input);
    filter->SetOnRatio(2);
    filter->SetOffset(100);
    filter->SetRandomMode(false);

    if (!filter->Execute()) { return false; }

    auto output = iGame::DynamicCast<iGame::UnstructuredMesh>(filter->GetOutput());

    if (output.IsNull()) { return false; }

    return output->GetNumberOfPoints() == 0;
}


int main() {
    bool allPassed = true;

    if (TestOnRatio()) {
        std::cout << "[PASS] OnRatio" << std::endl;
    } else {
        std::cout << "[FAIL] OnRatio" << std::endl;

        allPassed = false;
    }


    if (TestMaximumNumberOfPoints()) {
        std::cout << "[PASS] MaximumNumberOfPoints" << std::endl;
    } else {
        std::cout << "[FAIL] MaximumNumberOfPoints" << std::endl;

        allPassed = false;
    }


    if (TestOffset()) {
        std::cout << "[PASS] Offset" << std::endl;
    } else {
        std::cout << "[FAIL] Offset" << std::endl;

        allPassed = false;
    }


    if (TestRandomReproducibility()) {
        std::cout << "[PASS] RandomMode / RandomSeed" << std::endl;
    } else {
        std::cout << "[FAIL] RandomMode / RandomSeed" << std::endl;

        allPassed = false;
    }


    if (TestPointAttributes()) {
        std::cout << "[PASS] Point Attributes" << std::endl;
    } else {
        std::cout << "[FAIL] Point Attributes" << std::endl;

        allPassed = false;
    }


    if (TestAttributeTypes()) {
        std::cout << "[PASS] Attribute Types" << std::endl;
    } else {
        std::cout << "[FAIL] Attribute Types" << std::endl;

        allPassed = false;
    }


    if (TestEmptyInput()) {
        std::cout << "[PASS] Empty Input" << std::endl;
    } else {
        std::cout << "[FAIL] Empty Input" << std::endl;

        allPassed = false;
    }


    if (TestOffsetOutOfRange()) {
        std::cout << "[PASS] Offset Out Of Range" << std::endl;
    } else {
        std::cout << "[FAIL] Offset Out Of Range" << std::endl;

        allPassed = false;
    }


    if (!allPassed) {
        std::cout << "MaskPoints tests failed." << std::endl;

        return 1;
    }

    std::cout << "All MaskPoints tests passed." << std::endl;

    return 0;
}