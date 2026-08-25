#include <GhostCell/iGameGhostCellFilter.h>
#include <iGameAttributeSet.h>
#include <iGameFileIO.h>
#include <iGameSurfaceMesh.h>
#include <iGameType.h>

#include <iostream>

int main() {
	// 读取演示模型
	auto obj = iGame::FileIO::ReadFile("./Models/GhostDemo.vtk");
	if (obj.IsNull()) {
		std::cout << "READ FAILED" << std::endl;
		return 1;
	}
	auto mesh = iGame::DynamicCast<iGame::SurfaceMesh>(obj);
	if (mesh.IsNull()) {
		std::cout << "NOT A SURFACE MESH" << std::endl;
		return 1;
	}
	std::cout << "points = " << mesh->GetNumberOfPoints()
	          << ", faces = " << mesh->GetNumberOfFaces() << std::endl;

	// 运行 ghost cell filter
	auto filter = iGame::GhostCellFilter::New();
	filter->SetInput(0, mesh);
	bool ok = filter->Execute();
	std::cout << "Execute = " << (ok ? "true" : "false") << std::endl;

	// 读出 GhostCells 属性并逐个单元打印
	auto attrs = mesh->GetAttributeSet();
	int idx = attrs->GetAttributeIndex("GhostCells");
	std::cout << "GhostCells index = " << idx << std::endl;
	if (idx < 0) return 1;

	auto marker = iGame::DynamicCast<iGame::CharArray>(attrs->GetAttribute(idx).pointer);
	if (marker.IsNull()) {
		std::cout << "GhostCells is not a CharArray" << std::endl;
		return 1;
	}
	std::cout << "cell values:";
	for (IGsize i = 0; i < marker->GetNumberOfElements(); i++) {
		std::cout << " " << marker->GetValue(i);
	}
	std::cout << std::endl;
	return 0;
}
