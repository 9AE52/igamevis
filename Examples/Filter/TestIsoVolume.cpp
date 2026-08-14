#include <IsoVolume/iGameIsoVolumeFilter.h>
#include <iGameFileIO.h>
#include <iGameSurfaceMesh.h>
#include <iGameUnstructuredMesh.h>
#include <iGameVolumeMesh.h>
#include <iostream>

/* TestIsoVolume: 提取标量值落在 [lower, upper] 区间内的体网格
 * 输入: ./Models/Tet_Plane.vtk (带标量场的四面体网格)
 * 区间取标量范围的 [1/3, 2/3], 打印输出点数/单元数验证 */
int main() {

    /* 读取模型 */
    const std::string fileName = "./Models/Tet_Plane.vtk";
    iGame::DataObject::Pointer obj = iGame::FileIO::ReadFile(fileName);
    if (obj == nullptr) {
        std::cout << "Read ERROR!\n";
        return 1;
    }

    /* 取第一个点标量数组, 区间取数据范围的 [1/3, 2/3] */
    auto attrs = obj->GetAttributeSet()->GetAllPointAttributes();
    if (attrs == nullptr || attrs->GetNumberOfElements() == 0) {
        std::cout << "No point attributes ERROR!\n";
        return 1;
    }
    auto& attr = attrs->GetElement(0);
    auto range = attr.GetDataRange();
    auto array = attr.pointer;
    double lower = range->GetValue(0) + (range->GetValue(1) - range->GetValue(0)) / 3.0;
    double upper = range->GetValue(0) + (range->GetValue(1) - range->GetValue(0)) * 2.0 / 3.0;

    /* 等值面之间的体提取 */
    auto filter = iGame::IsoVolumeFilter::New();
    filter->SetInput(obj);
    filter->SetIsoScalarData(array, lower, upper, 0);
    filter->Execute();

    /* 打印结果 */
    auto res = filter->GetOutput();
    if (res == nullptr) {
        std::cout << "Output NULL ERROR!\n";
        return 1;
    }
    unsigned long long np = 0, nc = 0;
    if (auto m = iGame::DynamicCast<iGame::UnstructuredMesh>(res)) {
        np = m->GetNumberOfPoints();
        nc = m->GetNumberOfCells();
    } else if (auto m = iGame::DynamicCast<iGame::SurfaceMesh>(res)) {
        np = m->GetNumberOfPoints();
        nc = m->GetNumberOfFaces();
    } else if (auto m = iGame::DynamicCast<iGame::VolumeMesh>(res)) {
        np = m->GetNumberOfPoints();
        nc = m->GetNumberOfVolumes();
    }
    std::cout << "IsoVolume range = [" << lower << ", " << upper << "]\n";
    std::cout << "Input  scalar range = [" << range->GetValue(0) << ", " << range->GetValue(1) << "]\n";
    std::cout << "Output points = " << np << ", cells = " << nc << "\n";
    std::cout << "TestIsoVolume DONE\n";
    return 0;
}
