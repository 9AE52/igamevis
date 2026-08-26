#include <DataProcessing/iGamePointAndCellIdsFilter.h>

#include <iGameCellArray.h>
#include <iGameFileIO.h>
#include <iGameFlatArray.h>
#include <iGamePoints.h>
#include <iGameSurfaceMesh.h>

#include <iostream>
#include <string>

namespace
{

iGame::DataObject::Pointer CreateTestMesh() {
    auto points = iGame::Points::New();

    points->AddPoint(0.0f, 0.0f, 0.0f);
    points->AddPoint(1.0f, 0.0f, 0.0f);
    points->AddPoint(1.0f, 1.0f, 0.0f);
    points->AddPoint(0.0f, 1.0f, 0.0f);

    auto faces = iGame::CellArray::New();

    faces->AddCellId3(0, 1, 2);
    faces->AddCellId3(0, 2, 3);

    auto mesh = iGame::SurfaceMesh::New();

    mesh->SetPoints(points);
    mesh->SetFaces(faces);

    auto original = iGame::FloatArray::New();

    original->SetName("Original");
    original->SetDimension(1);
    original->Resize(4);

    for (IGsize i = 0; i < 4; ++i) { original->SetValue(i, static_cast<double>(i + 10)); }

    mesh->GetAttributeSet()->AddScalar(IG_POINT, original);

    return mesh;
}

iGame::DataObject::Pointer CreateInput(const char* fileName) {
    if (fileName != nullptr) { return iGame::FileIO::ReadFile(fileName); }

    return CreateTestMesh();
}

bool CheckIds(iGame::AttributeSet* attributes, IGenum attachmentType, const std::string& name, IGsize count) {
    auto* array = attributes->GetArrayPointer(IG_SCALAR, attachmentType, name);

    if (array == nullptr || array->GetNumberOfElements() != count) { return false; }

    for (IGsize i = 0; i < count; ++i) {

        if (array->GetValue(i) != static_cast<double>(i)) { return false; }
    }

    return true;
}

bool TestDefault(const char* fileName) {
    auto input = CreateInput(fileName);

    if (input == nullptr || input->GetPoints() == nullptr || input->GetCellArray() == nullptr) { return false; }

    const IGsize pointCount = input->GetPoints()->GetNumberOfPoints();

    const IGsize cellCount = input->GetCellArray()->GetNumberOfCells();

    auto* attributes = input->GetAttributeSet();

    const size_t before = attributes->GetNumberOfAttributes();

    auto filter = iGame::PointAndCellIdsFilter::New();

    filter->SetInput(input);

    if (!filter->Execute()) { return false; }

    if (!CheckIds(attributes, IG_POINT, "PointIds", pointCount) ||
        !CheckIds(attributes, IG_CELL, "CellIds", cellCount)) {

        return false;
    }

    const size_t after = attributes->GetNumberOfAttributes();

    if (after != before + 2) { return false; }

    if (!filter->Execute() || attributes->GetNumberOfAttributes() != after) { return false; }

    if (attributes->GetArrayPointer(IG_SCALAR, IG_POINT, "Original") == nullptr && fileName == nullptr) {

        return false;
    }

    std::cout << "[PASS] Default / repeat execution\n"
              << "       Points: " << pointCount << '\n'
              << "       Cells : " << cellCount << '\n';

    return true;
}

bool TestSwitches(const char* fileName) {

    auto pointInput = CreateInput(fileName);

    auto pointFilter = iGame::PointAndCellIdsFilter::New();

    pointFilter->SetInput(pointInput);
    pointFilter->SetGenerateCellIds(false);

    if (!pointFilter->Execute()) { return false; }

    auto* pointAttrs = pointInput->GetAttributeSet();

    if (!CheckIds(pointAttrs, IG_POINT, "PointIds", pointInput->GetPoints()->GetNumberOfPoints()) ||
        pointAttrs->GetArrayPointer(IG_SCALAR, IG_CELL, "CellIds") != nullptr) {

        return false;
    }

    auto cellInput = CreateInput(fileName);

    auto cellFilter = iGame::PointAndCellIdsFilter::New();

    cellFilter->SetInput(cellInput);
    cellFilter->SetGeneratePointIds(false);

    if (!cellFilter->Execute()) { return false; }

    auto* cellAttrs = cellInput->GetAttributeSet();

    if (!CheckIds(cellAttrs, IG_CELL, "CellIds", cellInput->GetCellArray()->GetNumberOfCells()) ||
        cellAttrs->GetArrayPointer(IG_SCALAR, IG_POINT, "PointIds") != nullptr) {

        return false;
    }

    std::cout << "[PASS] Point / Cell switches\n";

    return true;
}

bool TestCustomNames(const char* fileName) {
    auto input = CreateInput(fileName);

    auto filter = iGame::PointAndCellIdsFilter::New();

    filter->SetInput(input);

    filter->SetPointIdsArrayName("MyPointIds");
    filter->SetCellIdsArrayName("MyCellIds");

    if (!filter->Execute()) { return false; }

    auto* attributes = input->GetAttributeSet();

    const bool success = CheckIds(attributes, IG_POINT, "MyPointIds", input->GetPoints()->GetNumberOfPoints()) &&
                         CheckIds(attributes, IG_CELL, "MyCellIds", input->GetCellArray()->GetNumberOfCells());

    if (success) { std::cout << "[PASS] Custom array names\n"; }

    return success;
}

}


int main(int argc, char* argv[]) {
    const char* fileName = argc > 1 ? argv[1] : nullptr;

    if (fileName != nullptr) {
        std::cout << "Model: " << fileName << "\n\n";
    } else {
        std::cout << "Model: built-in test mesh\n\n";
    }

    const bool success = TestDefault(fileName) && TestSwitches(fileName) && TestCustomNames(fileName);

    if (!success) {
        std::cerr << "\nPointAndCellIdsFilter test FAILED.\n";

        return 1;
    }

    std::cout << "\nAll PointAndCellIdsFilter tests PASSED.\n";

    return 0;
}