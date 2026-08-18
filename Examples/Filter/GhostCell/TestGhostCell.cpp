#include <GhostCell/iGameGhostCellFilter.h>
#include <iGameAttributeSet.h>
#include <iGameFlatArray.h>
#include <iGameSurfaceMesh.h>
#include <iGameType.h>

#include <iostream>
#include <string>

namespace {

// 检查条件：成立打印 [PASS]，不成立打印 [FAIL] 并返回 false
bool Check(bool ok, const std::string& name) {
	if (ok) {
		std::cout << "[PASS] " << name << std::endl;
	} else {
		std::cout << "[FAIL] " << name << std::endl;
	}
	return ok;
}

// 步骤标记：每完成一步就打印一行，方便定位卡住的位置
void Step(const std::string& name) { std::cout << "  >> " << name << std::endl; }

// 场景一：两个三角形，其中第二个三角形包含一个 ghost 点
// 预期结果：面 0 = 正常(0)，面 1 = ghost(1)
bool TestWithPointGhosts() {
	std::cout << "== Test 1: mark one point as ghost ==" << std::endl;

	Step("create mesh");
	// 手工造一个正方形网格：4 个点
	//   3(0,1) ---- 2(1,1)
	//   |            |
	//   0(0,0) ---- 1(1,0)
	auto mesh = iGame::SurfaceMesh::New();

	Step("add 4 points");
	mesh->AddPoint(iGame::Point(0.0f, 0.0f, 0.0f));
	mesh->AddPoint(iGame::Point(1.0f, 0.0f, 0.0f));
	mesh->AddPoint(iGame::Point(1.0f, 1.0f, 0.0f));
	mesh->AddPoint(iGame::Point(0.0f, 1.0f, 0.0f));

	Step("add 2 faces");
	// 两个三角形：面 0 = (0,1,2)，面 1 = (0,2,3)
	// 注意：新网格不能直接调 AddFace（框架里"面容器"还没创建，会空指针崩溃），
	// 正确做法是先创建面容器再挂到网格上。
	igIndex tri0[3] = {0, 1, 2};
	igIndex tri1[3] = {0, 2, 3};
	auto faces = iGame::CellArray::New();
	mesh->SetFaces(faces);
	faces->AddCellIds(tri0, 3);
	faces->AddCellIds(tri1, 3);

	Step("create GhostPoints array");
	// 在"点"上挂一个 ghost 标记数组：只有 3 号点是 ghost 点（值 1）
	auto pointGhosts = iGame::CharArray::New();
	pointGhosts->SetName("GhostPoints");
	pointGhosts->Resize(4);
	pointGhosts->SetValue(0, 0.0);
	pointGhosts->SetValue(1, 0.0);
	pointGhosts->SetValue(2, 0.0);
	pointGhosts->SetValue(3, 1.0);
	mesh->GetAttributeSet()->AddScalar(IG_POINT, pointGhosts);

	Step("run filter");
	// 运行 filter
	auto filter = iGame::GhostCellFilter::New();
	filter->SetInput(0, mesh);
	if (!Check(filter->Execute(), "filter Execute()")) return false;

	Step("check result");
	// 检查输出：网格上应该多了一个"GhostCells"单元标记数组
	auto attrs = mesh->GetAttributeSet();
	int idx = attrs->GetAttributeIndex("GhostCells");
	if (!Check(idx >= 0, "GhostCells array exists")) return false;

	auto marker =
	    iGame::DynamicCast<iGame::CharArray>(attrs->GetAttribute(idx).pointer);
	if (!Check(!marker.IsNull(), "GhostCells is a CharArray")) return false;
	if (!Check(marker->GetNumberOfElements() == 2, "one value per cell"))
		return false;

	// 面 0 不含 ghost 点，面 1 含 ghost 点
	bool ok = true;
	ok &= Check(marker->GetValue(0) == 0.0, "cell 0 is normal (0)");
	ok &= Check(marker->GetValue(1) == 1.0, "cell 1 is ghost (1)");
	std::cout << "GhostCells = [ " << marker->GetValue(0) << ", "
	          << marker->GetValue(1) << " ]" << std::endl;
	return ok;
}

// 场景二：网格上根本没有 GhostPoints 标记
// 预期结果：filter 不崩溃，所有单元都标记为正常(0)
bool TestWithoutPointGhosts() {
	std::cout << "== Test 2: no GhostPoints array ==" << std::endl;

	Step("create mesh");
	auto mesh = iGame::SurfaceMesh::New();

	Step("add points and face");
	mesh->AddPoint(iGame::Point(0.0f, 0.0f, 0.0f));
	mesh->AddPoint(iGame::Point(1.0f, 0.0f, 0.0f));
	mesh->AddPoint(iGame::Point(1.0f, 1.0f, 0.0f));
	igIndex tri[3] = {0, 1, 2};
	auto faces = iGame::CellArray::New();
	mesh->SetFaces(faces);
	faces->AddCellIds(tri, 3);

	Step("run filter");
	auto filter = iGame::GhostCellFilter::New();
	filter->SetInput(0, mesh);
	if (!Check(filter->Execute(), "filter Execute()")) return false;

	Step("check result");
	auto attrs = mesh->GetAttributeSet();
	int idx = attrs->GetAttributeIndex("GhostCells");
	if (!Check(idx >= 0, "GhostCells array exists")) return false;
	auto marker =
	    iGame::DynamicCast<iGame::CharArray>(attrs->GetAttribute(idx).pointer);
	if (!Check(!marker.IsNull() && marker->GetNumberOfElements() == 1,
	           "one value per cell"))
		return false;

	bool ok = Check(marker->GetValue(0) == 0.0, "cell 0 is normal (0)");
	std::cout << "GhostCells = [ " << marker->GetValue(0) << " ]" << std::endl;
	return ok;
}

} // namespace

int main() {
	bool ok = true;
	ok &= TestWithPointGhosts();
	ok &= TestWithoutPointGhosts();

	if (ok) {
		std::cout << "\nALL TESTS PASSED" << std::endl;
		return 0;
	}
	std::cout << "\nSOME TESTS FAILED" << std::endl;
	return 1;
}
