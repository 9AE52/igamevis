#include <iostream>
#include <iGameFileIO.h>
#include <iGameUnstructuredMesh.h>
#include <ProcessGet/iGameGenerateProcessIdsFilter.h>

static bool Verify(iGame::UnstructuredMesh::Pointer mesh, bool pointData, IGsize expectCount, int expectValue) {
    auto filter = iGame::GenerateProcessIdsFilter::New();
    filter->SetInput(mesh);
    filter->SetGeneratePointData(pointData);
    filter->SetGenerateCellData(!pointData);
    filter->SetProcessId(expectValue);
    if (!filter->Execute()) {
        std::cout << "FAIL: Execute\n";
        return false;
    }
    auto& attr = mesh->GetAttributeSet()->GetScalar("process_ids");
    auto arr = attr.pointer;
    bool ok = (arr != nullptr) && (arr->GetNumberOfElements() == expectCount);
    for (IGsize i = 0; ok && i < expectCount; ++i) ok = (arr->GetValue(i) == expectValue);
    return ok;
}

int main(int argc, char* argv[]) {
    bool allOk = true;

    iGame::UnstructuredMesh::Pointer mesh;
    if (argc > 1) {
        auto obj = iGame::FileIO::ReadFile(argv[1]);
        mesh = iGame::DynamicCast<iGame::UnstructuredMesh>(obj);
        if (mesh == nullptr) {
            std::cout << "FAIL: read model " << argv[1] << "\n";
            return 1;
        }
    } else {
        mesh = iGame::UnstructuredMesh::New();
        mesh->AddPoint(iGame::Point(0.f, 0.f, 0.f));
        mesh->AddPoint(iGame::Point(1.f, 0.f, 0.f));
        mesh->AddPoint(iGame::Point(0.f, 1.f, 0.f));
        mesh->AddPoint(iGame::Point(0.f, 0.f, 1.f));
        igIndex cell[4] = {0, 1, 2, 3};
        mesh->AddCell(cell, 4, iGame::IG_TETRA);
    }

    IGsize pointNum = mesh->GetNumberOfPoints();
    bool pointOk = Verify(mesh, true, pointNum, 7);
    std::cout << (pointOk ? "PASS" : "FAIL") << ": point process_ids count=" << pointNum << " value=7\n";
    allOk = allOk && pointOk;

    auto cellMesh = iGame::UnstructuredMesh::New();
    cellMesh->AddPoint(iGame::Point(0.f, 0.f, 0.f));
    cellMesh->AddPoint(iGame::Point(1.f, 0.f, 0.f));
    cellMesh->AddPoint(iGame::Point(0.f, 1.f, 0.f));
    cellMesh->AddPoint(iGame::Point(0.f, 0.f, 1.f));
    igIndex cell[4] = {0, 1, 2, 3};
    cellMesh->AddCell(cell, 4, iGame::IG_TETRA);
    IGsize cellNum = cellMesh->GetNumberOfCells();
    bool cellOk = Verify(cellMesh, false, cellNum, 7);
    std::cout << (cellOk ? "PASS" : "FAIL") << ": cell process_ids count=" << cellNum << " value=7\n";
    allOk = allOk && cellOk;

    return allOk ? 0 : 1;
}
