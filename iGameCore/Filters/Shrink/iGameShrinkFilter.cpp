#include "iGameShrinkFilter.h"

#include "iGameCell.h" // 提供 IGAME_CELL_MAX_SIZE（单元最多顶点数）
#include "iGamePoints.h"

#include <algorithm>

IGAME_NAMESPACE_BEGIN

ShrinkFilter::ShrinkFilter() {
	SetNumberOfInputs(1);
	SetNumberOfOutputs(1);
}

void ShrinkFilter::SetShrinkFactor(double factor) {
	m_ShrinkFactor = std::clamp(factor, 0.0, 1.0);
}

double ShrinkFilter::GetShrinkFactor() const { return m_ShrinkFactor; }

bool ShrinkFilter::Execute() {
	auto input = GetInput(0);
	if (input.IsNull()) return false;

	// 核心逻辑：每个单元算出自己的质心，把它的顶点复制一份并向质心收缩，
	// 然后用新点替换单元的点编号。这样相邻单元会各自裂开，形成“爆炸图”效果。
	// pointSet：要被修改的网格；count：单元数量；cells：单元数组；
	// getCellPointIds：读取某个单元的顶点编号（每个网格类型实现不同）
	auto shrinkCells = [&](PointSet* pointSet, IGsize count, CellArray* cells,
	                       auto getCellPointIds) -> bool {
		if (pointSet == nullptr || cells == nullptr || count == 0) return false;

		auto oldPoints = pointSet->GetPoints();
		if (oldPoints.IsNull()) return false;

		// 1. 新建点容器，用来装每个单元“自己的”收缩后顶点
		auto newPoints = Points::New();

		igIndex ids[IGAME_CELL_MAX_SIZE];
		for (IGsize c = 0; c < count; c++) {
			int n = getCellPointIds(c, ids);
			if (n <= 0 || n > IGAME_CELL_MAX_SIZE) return false;

			// 2. 计算质心 = 单元所有顶点坐标的平均值
			double cx = 0.0, cy = 0.0, cz = 0.0;
			for (int k = 0; k < n; k++) {
				const auto& p = oldPoints->GetPoint(ids[k]);
				cx += p[0];
				cy += p[1];
				cz += p[2];
			}
			cx /= n;
			cy /= n;
			cz /= n;

			// 3. 每个顶点朝质心方向收缩，作为新点加入 newPoints
			igIndex newIds[IGAME_CELL_MAX_SIZE];
			for (int k = 0; k < n; k++) {
				const auto& p = oldPoints->GetPoint(ids[k]);
				float nx = static_cast<float>(cx + (p[0] - cx) * m_ShrinkFactor);
				float ny = static_cast<float>(cy + (p[1] - cy) * m_ShrinkFactor);
				float nz = static_cast<float>(cz + (p[2] - cz) * m_ShrinkFactor);
				newIds[k] = static_cast<igIndex>(newPoints->AddPoint(nx, ny, nz));
			}

			// 4. 把单元的顶点编号改成新点编号（单元数量、类型都不变）
			cells->SetCellIds(c, newIds, n);

			if ((c & 0x3FF) == 0) {
				UpdateProgress(static_cast<double>(c) / static_cast<double>(count));
			}
		}

		// 5. 用新点容器替换网格原来的点
		pointSet->SetPoints(newPoints);
		return true;
	};

	// 表面网格：单元 = 面
	if (auto mesh = DynamicCast<SurfaceMesh>(input)) {
		return shrinkCells(mesh, mesh->GetNumberOfFaces(), mesh->GetFaces(),
		                   [mesh](IGsize c, igIndex* ids) { return mesh->GetFacePointIds(c, ids); });
	}
	// 体网格：单元 = 体
	if (auto mesh = DynamicCast<VolumeMesh>(input)) {
		return shrinkCells(mesh, mesh->GetNumberOfVolumes(), mesh->GetVolumes(),
		                   [mesh](IGsize c, igIndex* ids) { return mesh->GetVolumePointIds(c, ids); });
	}
	// 非结构化网格（通用）
	if (auto mesh = DynamicCast<UnstructuredMesh>(input)) {
		return shrinkCells(mesh, mesh->GetNumberOfCells(), mesh->GetCellArray(),
		                   [mesh](IGsize c, igIndex* ids) { return mesh->GetCellPointIds(c, ids); });
	}
	return false;
}

IGAME_NAMESPACE_END
