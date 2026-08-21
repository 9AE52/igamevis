// ============================================================================
// ProbeFilter  — 在指定位置探测数据（点定位 + 插值）
// 由 Script/igame_new_filter.py 生成骨架后完成实现
//
// 算法说明（第一版）：
//   1. 点定位：用 nanoflann KD-tree 对输入数据点建索引，为每个探测点查找
//      K 个最近的数据点（K 由 SetNeighborCount 指定，默认 8）。
//   2. 插值：K 个近邻按反距离加权（IDW）插值输入的全部点属性（标量/矢量等）；
//      K=1 或探测点恰好落在数据点上时退化为最近点采样（不插值）。
//   3. 输出：结果 PointSet 与探测点一一对应，携带插值后的属性，另附
//      probe_distance（到最近数据点的距离）与 probe_located_point_id。
//
// 说明：该实现与输入类型无关（点云/面网格/体网格均可）；
//       后续如需更精确的"单元内重心插值"，可替换 Interpolate 相关逻辑。
// ============================================================================
#include "iGameProbeFilter.h"

#include <nanoflann.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

IGAME_NAMESPACE_BEGIN

namespace {

// 与 iGameVortexDetectionFilter.cpp 中同款 KD-tree 封装：K 近邻查询（L2 距离）
template <typename Scalar = float, typename Index = int32_t, int Dim = 3>
struct ProbeKDTree {
    static_assert(Dim > 0, "Dim must be positive");
    static_assert(std::is_floating_point<Scalar>::value, "Scalar must be floating point");
    static_assert(std::is_integral<Index>::value, "Index must be integral");

    struct PointCloud {
        std::vector<std::array<Scalar, Dim>> pts;

        inline size_t kdtree_get_point_count() const { return pts.size(); }

        inline Scalar kdtree_get_pt(const size_t idx, const size_t dim) const { return pts[idx][dim]; }

        template <class BBOX>
        bool kdtree_get_bbox(BBOX& bb) const {
            if (pts.empty()) return false;
            std::array<Scalar, Dim> lo = pts[0];
            std::array<Scalar, Dim> hi = pts[0];
            for (const auto& p : pts) {
                for (int d = 0; d < Dim; ++d) {
                    lo[d] = std::min(lo[d], p[d]);
                    hi[d] = std::max(hi[d], p[d]);
                }
            }
            for (int d = 0; d < Dim; ++d) {
                bb[d].low = lo[d];
                bb[d].high = hi[d];
            }
            return true;
        }
    };

    using Adaptor = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<Scalar, PointCloud>, PointCloud, Dim, Index>;

    PointCloud cloud;
    std::unique_ptr<Adaptor> index;

    void Build(size_t leafMaxSize = 32) {
        index = std::make_unique<Adaptor>(Dim, cloud,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(static_cast<int>(leafMaxSize)));
        index->buildIndex();
    }

    void Query(const Scalar q[Dim], int k, std::vector<Index>& result, std::vector<Scalar>& distances) const {
        result.clear();
        distances.clear();
        if (!index || cloud.pts.empty() || k <= 0) return;
        result.resize(k);
        distances.resize(k);
        nanoflann::KNNResultSet<Scalar, Index> rs(static_cast<size_t>(k));
        rs.init(result.data(), distances.data());
        nanoflann::SearchParameters sp;
        sp.sorted = true;
        index->findNeighbors(rs, q, sp);
        const size_t n = rs.size();
        result.resize(n);
        distances.resize(n);
    }
};

}  // namespace

ProbeFilter::ProbeFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(0);  // 结果通过 GetResult() 查询，与 Selection 系列一致
}

void ProbeFilter::SetProbePoints(PointSet::Pointer points) {
    m_ProbePoints = points;
}

void ProbeFilter::SetProbePoints(Points::Pointer points) {
    if (m_ProbePoints.IsNull()) m_ProbePoints = PointSet::New();
    m_ProbePoints->SetPoints(points);
}

IGsize ProbeFilter::AddProbePoint(const Point& point) {
    if (m_ProbePoints.IsNull()) m_ProbePoints = PointSet::New();
    return m_ProbePoints->AddPoint(point);
}

IGsize ProbeFilter::AddProbePoint(float x, float y, float z) {
    if (m_ProbePoints.IsNull()) m_ProbePoints = PointSet::New();
    return m_ProbePoints->AddPoint(Point(x, y, z));
}

IGsize ProbeFilter::GetNumberOfProbePoints() const {
    return m_ProbePoints.IsNull() ? 0 : m_ProbePoints->GetNumberOfPoints();
}

PointSet::Pointer ProbeFilter::GetResult() {
    return m_Result;
}

DoubleArray::Pointer ProbeFilter::GetProbeDistances() {
    return m_ProbeDistances;
}

IntArray::Pointer ProbeFilter::GetLocatedPointIds() {
    return m_LocatedPointIds;
}

bool ProbeFilter::Execute() {
    // ================= 取输入并校验 =================
    auto in = GetInput(0);
    if (in.IsNull()) return false;
    auto srcPoints = in->GetPoints();
    if (srcPoints.IsNull() || srcPoints->GetNumberOfPoints() == 0) return false;
    if (m_ProbePoints.IsNull() || m_ProbePoints->GetNumberOfPoints() == 0) return false;

    // 清空上一次结果，避免失败后返回旧数据
    m_Result = nullptr;
    m_ProbeDistances = nullptr;
    m_LocatedPointIds = nullptr;

    const IGsize numSrc = srcPoints->GetNumberOfPoints();
    const IGsize numProbes = m_ProbePoints->GetNumberOfPoints();
    const int k = std::max(1, m_NeighborCount);

    // ================= 1. 点定位：对输入数据点构建 KD-tree =================
    ProbeKDTree<> tree;
    tree.cloud.pts.resize(static_cast<size_t>(numSrc));
    for (IGsize i = 0; i < numSrc; ++i) {
        const Point& p = srcPoints->GetPoint(i);
        tree.cloud.pts[i] = {p[0], p[1], p[2]};
    }
    tree.Build(32);

    // ================= 2. 准备输出对象 =================
    m_Result = PointSet::New();
    m_Result->SetPoints(m_ProbePoints->GetPoints());

    m_ProbeDistances = DoubleArray::New();
    m_ProbeDistances->SetName("probe_distance");
    m_ProbeDistances->SetDimension(1);
    m_ProbeDistances->Resize(numProbes);

    m_LocatedPointIds = IntArray::New();
    m_LocatedPointIds->SetName("probe_located_point_id");
    m_LocatedPointIds->SetDimension(1);
    m_LocatedPointIds->Resize(numProbes);

    // 输入的全部点属性（标量/矢量等）按插值拷贝到结果点集
    struct OutAttribute {
        ArrayObject::Pointer inArray;
        FloatArray::Pointer outArray;
        int dimension;
    };
    std::vector<OutAttribute> outAttributes;
    AttributeSet* inAttributes = in->GetAttributeSet();
    if (inAttributes) {
        auto allAttributes = inAttributes->GetAllPointAttributes();
        for (IGsize i = 0; i < allAttributes->GetNumberOfElements(); ++i) {
            auto& attr = allAttributes->GetElement(i);
            if (attr.isDeleted || attr.pointer.IsNull()) continue;
            const int dimension = std::max(1, attr.pointer->GetDimension());
            auto outArray = FloatArray::New();
            outArray->SetName(attr.pointer->GetName());
            outArray->SetDimension(dimension);
            outArray->Resize(numProbes);
            m_Result->GetAttributeSet()->AddAttribute(attr.type, attr.attachmentType, outArray, attr.GetDataRange());
            outAttributes.push_back({attr.pointer, outArray, dimension});
        }
    }

    // ================= 3. 逐探测点：定位 + 插值 =================
    constexpr double kEpsDist2 = 1e-12;  // 距离平方接近 0 视为命中数据点
    std::vector<int32_t> neighborIds;
    std::vector<float> neighborDist2;
    double rawValues[IGAME_CELL_MAX_SIZE] = {};
    double interpolated[IGAME_CELL_MAX_SIZE] = {};

    for (IGsize probeId = 0; probeId < numProbes; ++probeId) {
        const Point& q = m_ProbePoints->GetPoint(probeId);
        const float query[3] = {q[0], q[1], q[2]};

        tree.Query(query, k, neighborIds, neighborDist2);
        if (neighborIds.empty()) {
            m_Result = nullptr;
            m_ProbeDistances = nullptr;
            m_LocatedPointIds = nullptr;
            return false;
        }

        // 记录定位信息：最近数据点 id 与距离
        m_LocatedPointIds->SetValue(probeId, neighborIds[0]);
        const double nearestDist2 = std::max(0.0, static_cast<double>(neighborDist2[0]));
        m_ProbeDistances->SetValue(probeId, std::sqrt(nearestDist2));

        // 恰好命中数据点（或 K=1）时退化为最近点采样，不做加权
        const bool exactHit = neighborDist2[0] <= kEpsDist2 || static_cast<int>(neighborIds.size()) == 1;

        for (auto& out : outAttributes) {
            const int dimension = out.dimension;
            for (int c = 0; c < dimension; ++c) interpolated[c] = 0.0;

            if (exactHit) {
                out.inArray->GetElement(neighborIds[0], rawValues);
                out.outArray->SetElement(probeId, rawValues);
                continue;
            }

            double sumWeight = 0.0;
            for (size_t j = 0; j < neighborIds.size(); ++j) {
                const double d2 = std::max(0.0, static_cast<double>(neighborDist2[j]));
                sumWeight += 1.0 / (d2 + kEpsDist2);
            }
            for (size_t j = 0; j < neighborIds.size(); ++j) {
                const double d2 = std::max(0.0, static_cast<double>(neighborDist2[j]));
                const double weight = 1.0 / (d2 + kEpsDist2);
                out.inArray->GetElement(neighborIds[j], rawValues);
                for (int c = 0; c < dimension; ++c) interpolated[c] += weight * rawValues[c];
            }
            for (int c = 0; c < dimension; ++c) interpolated[c] /= sumWeight;
            out.outArray->SetElement(probeId, interpolated);
        }

        UpdateProgress(static_cast<double>(probeId + 1) / static_cast<double>(numProbes));
    }

    m_Result->Modified();
    return true;
}

IGAME_NAMESPACE_END
