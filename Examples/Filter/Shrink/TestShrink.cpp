#include <Shrink/iGameShrinkFilter.h>
#include <iGameCellArray.h>
#include <iGamePoints.h>
#include <iGameSurfaceMesh.h>
#include <iGameType.h>

#include <cmath>
#include <iostream>
#include <string>

namespace {

double Dist(double x0, double y0, double z0, double x1, double y1, double z1) {
	double dx = x0 - x1;
	double dy = y0 - y1;
	double dz = z0 - z1;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool Check(bool ok, const std::string& name) {
	if (ok) {
		std::cout << "[PASS] " << name << std::endl;
	} else {
		std::cout << "[FAIL] " << name << std::endl;
	}
	return ok;
}

void Step(const std::string& name) { std::cout << "  >> " << name << std::endl; }

// 手工造一个正方形网格：4 个点、2 个三角形
iGame::SurfaceMesh::Pointer MakeSquareMesh() {
	auto mesh = iGame::SurfaceMesh::New();
	mesh->AddPoint(iGame::Point(0.0f, 0.0f, 0.0f)); // 0
	mesh->AddPoint(iGame::Point(1.0f, 0.0f, 0.0f)); // 1
	mesh->AddPoint(iGame::Point(1.0f, 1.0f, 0.0f)); // 2
	mesh->AddPoint(iGame::Point(0.0f, 1.0f, 0.0f)); // 3

	// 面 0 = (0,1,2)，面 1 = (0,2,3)
	auto faces = iGame::CellArray::New();
	mesh->SetFaces(faces);
	igIndex tri0[3] = {0, 1, 2};
	igIndex tri1[3] = {0, 2, 3};
	faces->AddCellIds(tri0, 3);
	faces->AddCellIds(tri1, 3);
	return mesh;
}

// 场景一：收缩比例 0.5
bool TestShrinkHalf() {
	std::cout << "== Test 1: shrink factor 0.5 ==" << std::endl;

	Step("create mesh");
	auto mesh = MakeSquareMesh();

	double orig[4][3] = {
	    {0.0, 0.0, 0.0},
	    {1.0, 0.0, 0.0},
	    {1.0, 1.0, 0.0},
	    {0.0, 1.0, 0.0},
	};

	Step("run filter");
	auto filter = iGame::ShrinkFilter::New();
	filter->SetShrinkFactor(0.5);
	filter->SetInput(0, mesh);
	if (!Check(filter->Execute(), "filter Execute()")) return false;

	Step("check result");
	auto pts = mesh->GetPoints();
	if (!Check(pts->GetNumberOfPoints() == 6, "each cell got its own vertices (4 -> 6 points)")) {
		return false;
	}
	if (!Check(mesh->GetNumberOfFaces() == 2, "face count unchanged (2)")) return false;

	bool ok = true;


	igIndex faceIds[2][3] = {{0, 1, 2}, {0, 2, 3}};
	for (int f = 0; f < 2; f++) {
		double cx = (orig[faceIds[f][0]][0] + orig[faceIds[f][1]][0] + orig[faceIds[f][2]][0]) / 3.0;
		double cy = (orig[faceIds[f][0]][1] + orig[faceIds[f][1]][1] + orig[faceIds[f][2]][1]) / 3.0;
		double cz = 0.0;

		igIndex newIds[3]{};
		int n = mesh->GetFacePointIds(f, newIds);
		if (!Check(n == 3, "face still has 3 vertices")) return false;

		for (int k = 0; k < 3; k++) {
			// 原始顶点到质心的距离
			const double* op = orig[faceIds[f][k]];
			double origDist = Dist(op[0], op[1], op[2], cx, cy, cz);
			// 收缩后顶点到质心的距离
			const auto& np = pts->GetPoint(newIds[k]);
			double newDist = Dist(np[0], np[1], np[2], cx, cy, cz);

			ok &= Check(std::fabs(newDist - 0.5 * origDist) < 1e-4,
			            "face " + std::to_string(f) + " vertex " + std::to_string(k) +
			                " moved halfway to centroid");
		}
	}

	igIndex f0[3]{}, f1[3]{};
	mesh->GetFacePointIds(0, f0);
	mesh->GetFacePointIds(1, f1);
	bool noShared = true;
	for (int i = 0; i < 3 && noShared; i++) {
		for (int j = 0; j < 3; j++) {
			if (f0[i] == f1[j]) {
				noShared = false;
				break;
			}
		}
	}
	ok &= Check(noShared, "the two faces no longer share vertices");
	return ok;
}

// 场景二：收缩比例 1.0
bool TestNoShrink() {
	std::cout << "== Test 2: shrink factor 1.0 ==" << std::endl;

	Step("create mesh");
	auto mesh = MakeSquareMesh();

	double orig[4][3] = {
	    {0.0, 0.0, 0.0},
	    {1.0, 0.0, 0.0},
	    {1.0, 1.0, 0.0},
	    {0.0, 1.0, 0.0},
	};

	Step("run filter");
	auto filter = iGame::ShrinkFilter::New();
	filter->SetShrinkFactor(1.0);
	filter->SetInput(0, mesh);
	if (!Check(filter->Execute(), "filter Execute()")) return false;

	Step("check result");
	auto pts = mesh->GetPoints();
	if (!Check(pts->GetNumberOfPoints() == 6, "vertices still duplicated (6 points)")) {
		return false;
	}

	igIndex faceIds[2][3] = {{0, 1, 2}, {0, 2, 3}};
	bool ok = true;
	for (int f = 0; f < 2; f++) {
		double cx = (orig[faceIds[f][0]][0] + orig[faceIds[f][1]][0] + orig[faceIds[f][2]][0]) / 3.0;
		double cy = (orig[faceIds[f][0]][1] + orig[faceIds[f][1]][1] + orig[faceIds[f][2]][1]) / 3.0;
		double cz = 0.0;

		igIndex newIds[3]{};
		mesh->GetFacePointIds(f, newIds);
		for (int k = 0; k < 3; k++) {
			const double* op = orig[faceIds[f][k]];
			double origDist = Dist(op[0], op[1], op[2], cx, cy, cz);
			const auto& np = pts->GetPoint(newIds[k]);
			double newDist = Dist(np[0], np[1], np[2], cx, cy, cz);
			ok &= Check(std::fabs(newDist - origDist) < 1e-4,
			            "face " + std::to_string(f) + " vertex " + std::to_string(k) +
			                " stays at original position");
		}
	}
	return ok;
}

} 

int main() {
	bool ok = true;
	ok &= TestShrinkHalf();
	ok &= TestNoShrink();

	if (ok) {
		std::cout << "\nALL TESTS PASSED" << std::endl;
		return 0;
	}
	std::cout << "\nSOME TESTS FAILED" << std::endl;
	return 1;
}
