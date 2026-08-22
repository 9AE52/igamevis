// ============================================================================
// TestProbe — ProbeFilter 重写后的最小验证
//
// 用例：
//   1. 四面体 + 已知线性标量场 f = x + 2y + 3z
//      - 体内点 (0.25, 0.25, 0.25)：应为单元线性插值，精确值 1.5；
//      - 顶点 (0,0,0)：应为单元命中，值 0；
//      - 体外点 (2,2,2)：应回退 IDW（method=1）。
//   2. 四边形面网格：探测四边形中心，面判断只认三角形 -> 应回退 IDW。
//   3. 六面体网格：探测体中心，体判断只认四面体 -> 应回退 IDW。
// ============================================================================
#include <Probe/iGameProbeFilter.h>

#include <iGameSurfaceMesh.h>
#include <iGameUnstructuredMesh.h>

#include <cmath>
#include <iostream>
#include <vector>

using namespace iGame;

namespace {

FloatArray::Pointer MakeScalar(const std::vector<double>& values) {
    auto arr = FloatArray::New();
    arr->SetName("f");
    arr->SetDimension(1);
    arr->Resize(static_cast<IGsize>(values.size()));
    for (IGsize i = 0; i < static_cast<IGsize>(values.size()); ++i) {
        arr->SetElement(i, &values[static_cast<size_t>(i)]);
    }
    return arr;
}

UnstructuredMesh::Pointer MakeTetraMesh() {
    auto mesh = UnstructuredMesh::New();
    mesh->AddPoint(Point(0.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(1.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(0.0f, 1.0f, 0.0f));
    mesh->AddPoint(Point(0.0f, 0.0f, 1.0f));

    igIndex ids[4] = {0, 1, 2, 3};
    mesh->AddCell(ids, 4, IG_TETRA);

    // f(x, y, z) = x + 2y + 3z
    mesh->GetAttributeSet()->AddScalar(IG_POINT, MakeScalar({0.0, 1.0, 2.0, 3.0}));
    return mesh;
}

SurfaceMesh::Pointer MakeQuadMesh() {
    auto mesh = SurfaceMesh::New();
    mesh->AddPoint(Point(0.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(1.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(1.0f, 1.0f, 0.0f));
    mesh->AddPoint(Point(0.0f, 1.0f, 0.0f));

    auto faces = CellArray::New();
    igIndex ids[4] = {0, 1, 2, 3};
    faces->AddCellIds(ids, 4);
    mesh->SetFaces(faces);

    mesh->GetAttributeSet()->AddScalar(IG_POINT, MakeScalar({0.0, 1.0, 2.0, 1.0}));
    return mesh;
}

UnstructuredMesh::Pointer MakeHexaMesh() {
    auto mesh = UnstructuredMesh::New();
    mesh->AddPoint(Point(0.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(1.0f, 0.0f, 0.0f));
    mesh->AddPoint(Point(1.0f, 1.0f, 0.0f));
    mesh->AddPoint(Point(0.0f, 1.0f, 0.0f));
    mesh->AddPoint(Point(0.0f, 0.0f, 1.0f));
    mesh->AddPoint(Point(1.0f, 0.0f, 1.0f));
    mesh->AddPoint(Point(1.0f, 1.0f, 1.0f));
    mesh->AddPoint(Point(0.0f, 1.0f, 1.0f));

    igIndex ids[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    mesh->AddCell(ids, 8, IG_HEXAHEDRON);
    mesh->GetAttributeSet()->AddScalar(IG_POINT, MakeScalar({0.0, 1.0, 2.0, 1.0,
                                                            1.0, 2.0, 3.0, 2.0}));
    return mesh;
}

// expectedMethod: 期望 probe_method（0=单元线性插值，1=IDW）
// expectedValue: >= -1e10 时同时校验插值精度
bool ProbeAndCheck(DataObject::Pointer obj, const Point& q, int expectedMethod,
                   double expectedValue, const char* name) {
    auto filter = ProbeFilter::New();
    filter->SetInput(obj);
    filter->SetProbePoint(q);
    filter->SetNeighborCount(8);
    if (!filter->Execute()) {
        std::cout << "[FAIL] " << name << ": Execute() returned false" << std::endl;
        return false;
    }

    auto result = filter->GetResult();
    if (result.IsNull()) {
        std::cout << "[FAIL] " << name << ": result is null" << std::endl;
        return false;
    }

    const int method = filter->GetProbeMethods()->GetValue(0);

    double value = 0.0;
    bool hasValue = false;
    auto attributes = result->GetAttributeSet()->GetAllPointAttributes();
    for (IGsize i = 0; i < attributes->GetNumberOfElements(); ++i) {
        auto& attr = attributes->GetElement(i);
        if (attr.isDeleted || attr.pointer.IsNull()) continue;
        double values[IGAME_CELL_MAX_SIZE] = {};
        attr.pointer->GetElement(0, values);
        value = values[0];
        hasValue = true;
        break;
    }

    bool ok = (method == expectedMethod) && hasValue;
    if (expectedValue > -1e10) {
        ok = ok && std::fabs(value - expectedValue) < 1e-4;
    }

    std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << ": method=" << method
              << " value=" << value << std::endl;
    return ok;
}

}  // namespace

int main() {
    bool allPassed = true;

    auto tetra = MakeTetraMesh();
    allPassed &= ProbeAndCheck(tetra, Point(0.25f, 0.25f, 0.25f), 0, 1.5,
                               "tetra inside");
    allPassed &= ProbeAndCheck(tetra, Point(0.0f, 0.0f, 0.0f), 0, 0.0,
                               "tetra vertex");
    allPassed &= ProbeAndCheck(tetra, Point(2.0f, 2.0f, 2.0f), 1, -1e30,
                               "tetra outside -> IDW");

    auto quad = MakeQuadMesh();
    allPassed &= ProbeAndCheck(quad, Point(0.5f, 0.5f, 0.0f), 1, -1e30,
                               "quad center -> IDW");

    auto hexa = MakeHexaMesh();
    allPassed &= ProbeAndCheck(hexa, Point(0.5f, 0.5f, 0.5f), 1, -1e30,
                               "hexa center -> IDW");

    std::cout << (allPassed ? "[RESULT] all passed" : "[RESULT] some failed")
              << std::endl;
    return allPassed ? 0 : 1;
}
