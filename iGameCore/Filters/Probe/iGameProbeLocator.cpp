// ============================================================================
// ProbeLocator — 见 iGameProbeLocator.h
//
// 三角形包含：投影到法向最大的坐标平面，用 2D 有向面积求重心坐标；
//             另加平面距离检查，容忍浮点误差造成的微小偏离。
// 四面体包含：用有符号体积求重心坐标。
// 两者都使用“含边界”判定（重心坐标 >= -eps）。
// ============================================================================
#include "iGameProbeLocator.h"

#include <algorithm>
#include <cmath>

IGAME_NAMESPACE_BEGIN

namespace {

// 若干点的包围盒对角线（用于把无量纲容差换算成几何容差）
double BoundingDiag(const Point* pts, int n) {
    if (n <= 0) return 0.0;
    double lo[3] = {pts[0][0], pts[0][1], pts[0][2]};
    double hi[3] = {pts[0][0], pts[0][1], pts[0][2]};
    for (int i = 1; i < n; ++i) {
        for (int d = 0; d < 3; ++d) {
            lo[d] = std::min(lo[d], static_cast<double>(pts[i][d]));
            hi[d] = std::max(hi[d], static_cast<double>(pts[i][d]));
        }
    }
    double diag2 = 0.0;
    for (int d = 0; d < 3; ++d) {
        const double diff = hi[d] - lo[d];
        diag2 += diff * diff;
    }
    return std::sqrt(diag2);
}

// 四面体有符号体积（1/6 * 混合积）
double SignedTetraVolume(const Point& a, const Point& b, const Point& c,
                         const Point& d) {
    const Vector3f ab = b - a;
    const Vector3f ac = c - a;
    const Vector3f ad = d - a;
    return ab.cross(ac).dot(ad) / 6.0;
}

bool BarycentricInside(const double* lambda, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        if (lambda[i] < -kProbeBarycentricEps ||
            lambda[i] > 1.0 + kProbeBarycentricEps) {
            return false;
        }
        sum += lambda[i];
    }
    return std::fabs(sum - 1.0) <= kProbeBarycentricEps;
}

}  // namespace

bool ProbePointInTriangle(const Point& q, const Point& a, const Point& b,
                          const Point& c, double lambda[3]) {
    lambda[0] = lambda[1] = lambda[2] = 0.0;

    const Point pts[3] = {a, b, c};
    const double diag = BoundingDiag(pts, 3);
    const double geomEps = std::max(kProbeNumericalEps, kProbeBarycentricEps * diag);

    const Vector3f ab = b - a;
    const Vector3f ac = c - a;
    const Vector3f normal = ab.cross(ac);
    const double normalLen2 = normal.squaredLength();
    if (normalLen2 <= kProbeNumericalEps) return false;  // 退化三角形
    const double normalLen = std::sqrt(normalLen2);

    // 平面距离检查：点必须基本落在三角形所在平面上
    const Vector3f aq = q - a;
    if (std::fabs(normal.dot(aq)) / normalLen > geomEps) return false;

    // 投影到法向分量最大的坐标平面（去掉该轴），用 2D 有向面积求重心坐标
    int axis = 0;
    if (std::fabs(normal[1]) > std::fabs(normal[axis])) axis = 1;
    if (std::fabs(normal[2]) > std::fabs(normal[axis])) axis = 2;
    const int u = (axis + 1) % 3;
    const int v = (axis + 2) % 3;

    const auto area2 = [&](const Point& p0, const Point& p1, const Point& p2) {
        const double x0 = p0[u], y0 = p0[v];
        const double x1 = p1[u], y1 = p1[v];
        const double x2 = p2[u], y2 = p2[v];
        return (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
    };

    const double denom = area2(a, b, c);
    if (std::fabs(denom) <= kProbeNumericalEps) return false;

    lambda[0] = area2(q, b, c) / denom;
    lambda[1] = area2(a, q, c) / denom;
    lambda[2] = area2(a, b, q) / denom;
    return BarycentricInside(lambda, 3);
}

bool ProbePointInTetra(const Point& q, const Point& a, const Point& b,
                       const Point& c, const Point& d, double lambda[4]) {
    lambda[0] = lambda[1] = lambda[2] = lambda[3] = 0.0;

    const double vol = SignedTetraVolume(a, b, c, d);
    if (std::fabs(vol) <= kProbeNumericalEps) return false;  // 退化四面体

    lambda[0] = SignedTetraVolume(q, b, c, d) / vol;
    lambda[1] = SignedTetraVolume(a, q, c, d) / vol;
    lambda[2] = SignedTetraVolume(a, b, q, d) / vol;
    lambda[3] = SignedTetraVolume(a, b, c, q) / vol;
    return BarycentricInside(lambda, 4);
}

bool ProbeLocateInFace(const Cell* cell, const Point& q, ProbeSimplexHit& hit) {
    hit = ProbeSimplexHit{};
    if (cell == nullptr || cell->GetCellType() != IG_TRIANGLE) return false;

    double lambda[3] = {0.0, 0.0, 0.0};
    if (!ProbePointInTriangle(q, cell->GetPoint(0), cell->GetPoint(1),
                              cell->GetPoint(2), lambda)) {
        return false;
    }

    hit.found = true;
    hit.numVertices = 3;
    hit.localVertIds[0] = 0;
    hit.localVertIds[1] = 1;
    hit.localVertIds[2] = 2;
    hit.barycentric[0] = lambda[0];
    hit.barycentric[1] = lambda[1];
    hit.barycentric[2] = lambda[2];
    return true;
}

bool ProbeLocateInVolume(const Cell* cell, const Point& q, ProbeSimplexHit& hit) {
    hit = ProbeSimplexHit{};
    if (cell == nullptr || cell->GetCellType() != IG_TETRA) return false;

    double lambda[4] = {0.0, 0.0, 0.0, 0.0};
    if (!ProbePointInTetra(q, cell->GetPoint(0), cell->GetPoint(1),
                           cell->GetPoint(2), cell->GetPoint(3), lambda)) {
        return false;
    }

    hit.found = true;
    hit.numVertices = 4;
    hit.localVertIds[0] = 0;
    hit.localVertIds[1] = 1;
    hit.localVertIds[2] = 2;
    hit.localVertIds[3] = 3;
    hit.barycentric[0] = lambda[0];
    hit.barycentric[1] = lambda[1];
    hit.barycentric[2] = lambda[2];
    hit.barycentric[3] = lambda[3];
    return true;
}

IGAME_NAMESPACE_END
