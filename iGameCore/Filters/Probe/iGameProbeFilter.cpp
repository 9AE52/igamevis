// ============================================================================
// ProbeFilter — 见 iGameProbeFilter.h
//
// 算法说明（第二版）：
//   1. 单元定位：遍历所有单元。面单元走 ProbeLocateInFace（只认三角形）、
//      体单元走 ProbeLocateInVolume（只认四面体）；命中后按重心坐标对全部
//      点属性（标量/矢量等）做线性插值，probe_method = 0。
//   2. IDW 回退：所有单元都未命中（或输入没有单元）时，遍历全部数据点，
//      用最大堆维护距离最近的 k 个点（O(n log k)），按 1/(d^2 + eps) 加权
//      插值，probe_method = 1。
// ============================================================================
#include "iGameProbeFilter.h"

#include "iGameProbeLocator.h"

#include <iGameSurfaceMesh.h>
#include <iGameUnstructuredMesh.h>
#include <iGameVolumeMesh.h>

#include <algorithm>
#include <cstdint>
#include <queue>
#include <utility>
#include <vector>

IGAME_NAMESPACE_BEGIN

ProbeFilter::ProbeFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(0);  // 结果通过 GetResult() 查询，与 Selection 系列一致
}

void ProbeFilter::SetProbePoint(const Point& point) {
    m_ProbePoint = point;
    m_HasProbePoint = true;
}

void ProbeFilter::SetProbePoint(float x, float y, float z) {
    m_ProbePoint = Point(x, y, z);
    m_HasProbePoint = true;
}

PointSet::Pointer ProbeFilter::GetResult() {
    return m_Result;
}

IntArray::Pointer ProbeFilter::GetProbeMethods() {
    return m_ProbeMethods;
}

bool ProbeFilter::Execute() {
    // ================= 取输入并校验 =================
    auto in = GetInput(0);
    if (in.IsNull()) return false;
    auto srcPoints = in->GetPoints();
    if (srcPoints.IsNull() || srcPoints->GetNumberOfPoints() == 0) return false;
    if (!m_HasProbePoint) return false;

    // 清空上一次结果，避免失败后返回旧数据
    m_Result = nullptr;
    m_ProbeMethods = nullptr;

    const IGsize numSrc = srcPoints->GetNumberOfPoints();
    const int k = std::max(1, m_NeighborCount);
    const Point q = m_ProbePoint;

    // ================= 输出对象（1 个探测点）=================
    m_Result = PointSet::New();
    auto outPoints = Points::New();
    outPoints->AddPoint(q);
    m_Result->SetPoints(outPoints);

    m_ProbeMethods = IntArray::New();
    m_ProbeMethods->SetName("probe_method");
    m_ProbeMethods->SetDimension(1);
    m_ProbeMethods->Resize(1);

    // 输入的全部点属性（标量/矢量等）按插值拷贝到结果点集
    struct OutAttribute {
        ArrayObject::Pointer inArray;
        FloatArray::Pointer outArray;
        int dimension;
    };
    std::vector<OutAttribute> outAttributes;
    if (AttributeSet* inAttributes = in->GetAttributeSet()) {
        auto allAttributes = inAttributes->GetAllPointAttributes();
        for (IGsize i = 0; i < allAttributes->GetNumberOfElements(); ++i) {
            auto& attr = allAttributes->GetElement(i);
            if (attr.isDeleted || attr.pointer.IsNull()) continue;
            const int dimension = std::max(1, attr.pointer->GetDimension());
            auto outArray = FloatArray::New();
            outArray->SetName(attr.pointer->GetName());
            outArray->SetDimension(dimension);
            outArray->Resize(1);
            m_Result->GetAttributeSet()->AddAttribute(attr.type, attr.attachmentType,
                                                      outArray, attr.GetDataRange());
            outAttributes.push_back({attr.pointer, outArray, dimension});
        }
    }

    // 对全部点属性做一次带权组合：value(q) = Σ w_i * value(pointId_i)
    const auto interpolateWith =
        [&outAttributes](const std::vector<std::pair<igIndex, double>>& weightedPoints) {
            double rawValues[IGAME_CELL_MAX_SIZE] = {};
            double interpolated[IGAME_CELL_MAX_SIZE] = {};
            for (auto& out : outAttributes) {
                const int dimension = out.dimension;
                for (int c = 0; c < dimension; ++c) interpolated[c] = 0.0;
                for (const auto& wp : weightedPoints) {
                    out.inArray->GetElement(wp.first, rawValues);
                    for (int c = 0; c < dimension; ++c) {
                        interpolated[c] += wp.second * rawValues[c];
                    }
                }
                out.outArray->SetElement(0, interpolated);
            }
        };

    // ================= 1. 单元定位 =================
    Cell* hitCell = nullptr;
    ProbeSimplexHit hit;

    auto cellArray = in->GetCellArray();
    if (cellArray && cellArray->GetNumberOfCells() > 0) {
        auto um = DynamicCast<UnstructuredMesh>(in);
        auto vm = DynamicCast<VolumeMesh>(in);
        auto sm = DynamicCast<SurfaceMesh>(in);

        const IGsize numCells = cellArray->GetNumberOfCells();
        for (IGsize cellId = 0; cellId < numCells; ++cellId) {
            Cell* cell = nullptr;
            if (um) {
                cell = um->GetCell(cellId);
            } else if (vm) {
                cell = vm->GetVolume(cellId);
            } else if (sm) {
                cell = sm->GetFace(cellId);
            }
            if (cell == nullptr) continue;

            const auto cellType = static_cast<IGCellType>(cell->GetCellType());
            const igIndex dim = Cell::GetCellDimension(cellType);
            const bool found = (dim == 2) ? ProbeLocateInFace(cell, q, hit)
                                          : (dim == 3) ? ProbeLocateInVolume(cell, q, hit)
                                                       : false;
            if (found) {
                hitCell = cell;
                break;
            }
        }
    }

    // ================= 2. 插值 =================
    if (hitCell != nullptr && hit.found) {
        // ---- 单元线性插值 ----
        m_ProbeMethods->SetValue(0, 0);

        std::vector<std::pair<igIndex, double>> weightedPoints;
        weightedPoints.reserve(hit.numVertices);
        for (int v = 0; v < hit.numVertices; ++v) {
            weightedPoints.emplace_back(hitCell->GetPointId(hit.localVertIds[v]),
                                        hit.barycentric[v]);
        }
        interpolateWith(weightedPoints);
    } else {
        // ---- IDW 回退：最大堆维护最近 k 个点 ----
        m_ProbeMethods->SetValue(0, 1);

        struct Neighbor {
            double dist2;
            int32_t id;
            bool operator<(const Neighbor& o) const {
                if (dist2 != o.dist2) return dist2 < o.dist2;
                return id < o.id;
            }
        };

        std::priority_queue<Neighbor> heap;
        for (IGsize i = 0; i < numSrc; ++i) {
            const Point& p = srcPoints->GetPoint(i);
            const double dx = static_cast<double>(q[0]) - p[0];
            const double dy = static_cast<double>(q[1]) - p[1];
            const double dz = static_cast<double>(q[2]) - p[2];
            Neighbor nb{dx * dx + dy * dy + dz * dz, static_cast<int32_t>(i)};
            if (static_cast<int>(heap.size()) < k) {
                heap.push(nb);
            } else if (nb < heap.top()) {
                heap.pop();
                heap.push(nb);
            }
        }

        std::vector<Neighbor> knn;
        knn.reserve(heap.size());
        while (!heap.empty()) {
            knn.push_back(heap.top());
            heap.pop();
        }
        std::sort(knn.begin(), knn.end());  // 按 (距离², id) 升序，保证确定性

        if (knn.empty()) {
            m_Result = nullptr;
            m_ProbeMethods = nullptr;
            return false;
        }

        constexpr double kEpsDist2 = 1e-12;
        std::vector<std::pair<igIndex, double>> weightedPoints;
        // 恰好命中数据点（或 K=1）时退化为最近点采样，不做加权
        if (knn[0].dist2 <= kEpsDist2 || knn.size() == 1) {
            weightedPoints.emplace_back(knn[0].id, 1.0);
        } else {
            double sumWeight = 0.0;
            for (const auto& nb : knn) {
                sumWeight += 1.0 / (nb.dist2 + kEpsDist2);
            }
            for (const auto& nb : knn) {
                weightedPoints.emplace_back(nb.id, 1.0 / (nb.dist2 + kEpsDist2) / sumWeight);
            }
        }
        interpolateWith(weightedPoints);
    }

    m_Result->Modified();
    return true;
}

IGAME_NAMESPACE_END
