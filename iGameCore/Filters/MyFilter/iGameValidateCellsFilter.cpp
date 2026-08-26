#include "iGameValidateCellsFilter.h"

#include "iGameAttributeSet.h"
#include "iGameFlatArray.h"

#include <cmath>
#include <algorithm>
#include <array>

IGAME_NAMESPACE_BEGIN

namespace {

// 默认容差，与 VTK 保持一致
constexpr double kTol = 1.1920929e-7;

// ===========================================================================
// 拓扑定义（完全匹配 VTK 单元顺序）
// ===========================================================================

// --- 四面体 (4点) ---
constexpr int kTetraEdges[6][2] = {
    {0,1},{1,2},{0,2},{0,3},{1,3},{2,3}
};
const std::vector<std::vector<int>> kTetraFaces = {
    {0,1,3},{1,2,3},{0,3,2},{0,2,1}
};

// --- 六面体 (8点) ---
constexpr int kHexEdges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};
const std::vector<std::vector<int>> kHexFaces = {
    {0,4,7,3},{1,2,6,5},{0,1,5,4},{3,7,6,2},{0,3,2,1},{4,5,6,7}
};

// --- 楔形/棱柱 (6点) ---
constexpr int kWedgeEdges[9][2] = {
    {0,1},{1,2},{0,2},
    {3,4},{4,5},{3,5},
    {0,3},{1,4},{2,5}
};
const std::vector<std::vector<int>> kWedgeFaces = {
    {0,2,1},{3,4,5},{0,1,4,3},{1,2,5,4},{2,0,3,5}
};

// --- 金字塔 (5点) ---
constexpr int kPyramidEdges[8][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {0,4},{1,4},{2,4},{3,4}
};
const std::vector<std::vector<int>> kPyramidFaces = {
    {0,3,2,1},{0,1,4},{1,2,4},{2,3,4},{3,0,4}
};

// ===========================================================================
// 几何辅助函数 —— 移植自 VTK vtkCellValidator / vtkLine / vtkTriangle
// ===========================================================================

inline bool PointsAreCoincident(const Point& a, const Point& b, double tol) {
    return std::abs(static_cast<double>(a[0] - b[0])) < tol &&
           std::abs(static_cast<double>(a[1] - b[1])) < tol &&
           std::abs(static_cast<double>(a[2] - b[2])) < tol;
}

// 3D 线段相交检测（非共享端点）
bool SegmentsIntersect(const Point& p1, const Point& p2,
                       const Point& q1, const Point& q2, double tol) {
    Vector3f a = p2 - p1;
    Vector3f b = q2 - q1;
    Vector3f w = p1 - q1;
    double aa = static_cast<double>(DotProduct(a, a));
    double bb = static_cast<double>(DotProduct(b, b));
    double ab = static_cast<double>(DotProduct(a, b));
    double aw = static_cast<double>(DotProduct(a, w));
    double bw = static_cast<double>(DotProduct(b, w));
    double denom = aa * bb - ab * ab;
    if (std::abs(denom) < 1e-30) return false;  // 平行
    double u = (ab * bw - bb * aw) / denom;
    double v = (aa * bw - ab * aw) / denom;
    if (u < -tol || u > 1.0 + tol || v < -tol || v > 1.0 + tol) return false;
    Point c1 = p1 + a * static_cast<float>(u);
    Point c2 = q1 + b * static_cast<float>(v);
    if (static_cast<double>((c1 - c2).norm()) > tol) return false;
    u = std::max(0.0, std::min(1.0, u));
    v = std::max(0.0, std::min(1.0, v));
    bool uInterior = (std::abs(u) > tol && std::abs(u - 1.0) > tol);
    bool vInterior = (std::abs(v) > tol && std::abs(v - 1.0) > tol);
    return uInterior || vInterior;
}

// Möller 三角形-三角形相交测试（非共面）
bool MollerTriTri(const Point& v0, const Point& v1, const Point& v2,
                  const Point& u0, const Point& u1, const Point& u2) {
    // 平面 V
    Vector3f eV1 = v1 - v0;
    Vector3f eV2 = v2 - v0;
    Vector3f nV = CrossProduct(eV1, eV2);
    double dV = -static_cast<double>(DotProduct(nV, v0));
    double du0 = static_cast<double>(DotProduct(nV, u0)) + dV;
    double du1 = static_cast<double>(DotProduct(nV, u1)) + dV;
    double du2 = static_cast<double>(DotProduct(nV, u2)) + dV;
    if (du0 > 0.0 && du1 > 0.0 && du2 > 0.0) return false;
    if (du0 < 0.0 && du1 < 0.0 && du2 < 0.0) return false;

    // 平面 U
    Vector3f eU1 = u1 - u0;
    Vector3f eU2 = u2 - u0;
    Vector3f nU = CrossProduct(eU1, eU2);
    double dU = -static_cast<double>(DotProduct(nU, u0));
    double dv0 = static_cast<double>(DotProduct(nU, v0)) + dU;
    double dv1 = static_cast<double>(DotProduct(nU, v1)) + dU;
    double dv2 = static_cast<double>(DotProduct(nU, v2)) + dU;
    if (dv0 > 0.0 && dv1 > 0.0 && dv2 > 0.0) return false;
    if (dv0 < 0.0 && dv1 < 0.0 && dv2 < 0.0) return false;

    double nVnU = static_cast<double>(DotProduct(nV, nU));
    if (std::abs(nVnU) < 1e-25) return false; // 共面（或近似共面）交给上层处理
    Vector3f dir = CrossProduct(nV, nU);

    auto edgePlanePoint = [](const Point& a, const Point& b, double da, double db) -> Point {
        double t = da / (da - db);
        return a + (b - a) * static_cast<float>(t);
    };

    // V 在平面 U 上的投影区间
    double vVals[2];
    int cntV = 0;
    const Point* vPts[3] = {&v0, &v1, &v2};
    double vD[3] = {dv0, dv1, dv2};
    for (int i = 0; i < 3 && cntV < 2; ++i) {
        int j = (i + 1) % 3;
        if ((vD[i] > 0.0 && vD[j] <= 0.0) || (vD[i] < 0.0 && vD[j] >= 0.0)) {
            Point p = edgePlanePoint(*vPts[i], *vPts[j], vD[i], vD[j]);
            vVals[cntV++] = static_cast<double>(DotProduct(p, dir));
        }
    }
    if (cntV < 2) return false;

    // U 在平面 V 上的投影区间
    double uVals[2];
    int cntU = 0;
    const Point* uPts[3] = {&u0, &u1, &u2};
    double uD[3] = {du0, du1, du2};
    for (int i = 0; i < 3 && cntU < 2; ++i) {
        int j = (i + 1) % 3;
        if ((uD[i] > 0.0 && uD[j] <= 0.0) || (uD[i] < 0.0 && uD[j] >= 0.0)) {
            Point p = edgePlanePoint(*uPts[i], *uPts[j], uD[i], uD[j]);
            uVals[cntU++] = static_cast<double>(DotProduct(p, dir));
        }
    }
    if (cntU < 2) return false;

    if (vVals[0] > vVals[1]) std::swap(vVals[0], vVals[1]);
    if (uVals[0] > uVals[1]) std::swap(uVals[0], uVals[1]);
    return vVals[0] <= uVals[1] && uVals[0] <= vVals[1];
}

// -------------------- 新增辅助函数（用于增强检测） --------------------

// 计算三角形面积2倍（叉积模平方），用于检测共线
inline double TriangleArea2Sq(const Point& a, const Point& b, const Point& c) {
    Vector3f v1 = b - a;
    Vector3f v2 = c - a;
    Vector3f n = CrossProduct(v1, v2);
    return static_cast<double>(DotProduct(n, n));
}

// 计算四面体体积（带符号）
inline double TetraSignedVolume(const Point& a, const Point& b, const Point& c, const Point& d) {
    Vector3f ab = b - a, ac = c - a, ad = d - a;
    Vector3f cr = CrossProduct(ac, ad);
    return static_cast<double>(DotProduct(ab, cr)) / 6.0;
}

// 检测点集中是否存在重复点（距离 < tol）
bool HasDuplicatePoints(const std::vector<Point>& pts, double tol) {
    for (size_t i = 0; i < pts.size(); ++i)
        for (size_t j = i + 1; j < pts.size(); ++j)
            if ((pts[i] - pts[j]).norm() < tol) return true;
    return false;
}

// 判断点是否在三角形内部（严格内部，忽略边界）
bool PointInTriangleInterior(const Point& p, const Point& a, const Point& b, const Point& c, double tol) {
    Vector3f v0 = c - a, v1 = b - a, v2 = p - a;
    double dot00 = DotProduct(v0, v0);
    double dot01 = DotProduct(v0, v1);
    double dot02 = DotProduct(v0, v2);
    double dot11 = DotProduct(v1, v1);
    double dot12 = DotProduct(v1, v2);
    double denom = dot00 * dot11 - dot01 * dot01;
    if (std::abs(denom) < 1e-30) return false;
    double u = (dot11 * dot02 - dot01 * dot12) / denom;
    double v = (dot00 * dot12 - dot01 * dot02) / denom;
    // 严格内部（忽略边界，避免与边相交重复报告）
    const double eps = 1e-7;
    return (u > eps && v > eps && (u + v) < 1.0 - eps);
}

// 检测两个共面三角形是否重叠（面积重叠）
bool CoplanarTrianglesOverlap(const Point& a1, const Point& a2, const Point& a3,
                              const Point& b1, const Point& b2, const Point& b3,
                              double tol) {
    // 检查是否有边相交（内部点）
    const Point* a[3] = {&a1, &a2, &a3};
    const Point* b[3] = {&b1, &b2, &b3};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (SegmentsIntersect(*a[i], *a[(i+1)%3], *b[j], *b[(j+1)%3], tol))
                return true; // 边交叉视为重叠
    // 点包含测试
    if (PointInTriangleInterior(a1, b1, b2, b3, tol) ||
        PointInTriangleInterior(a2, b1, b2, b3, tol) ||
        PointInTriangleInterior(a3, b1, b2, b3, tol) ||
        PointInTriangleInterior(b1, a1, a2, a3, tol) ||
        PointInTriangleInterior(b2, a1, a2, a3, tol) ||
        PointInTriangleInterior(b3, a1, a2, a3, tol))
        return true;
    return false;
}

// -------------------- 几何检测函数（修改） --------------------

// 检查所有非相邻边是否相交（仅检测内部交点，不包含共享顶点）
bool NoIntersectingEdges(const std::vector<Point>& pts,
                         const int edges[][2], int nEdges, double tol) {
    for (int i = 0; i < nEdges; ++i) {
        for (int j = i + 1; j < nEdges; ++j) {
            if (SegmentsIntersect(pts[edges[i][0]], pts[edges[i][1]],
                                  pts[edges[j][0]], pts[edges[j][1]], tol)) {
                return false;
            }
        }
    }
    return true;
}

// 三角剖分多边形（扇形从顶点0）
std::vector<std::array<int,3>> TriangulateFace(const std::vector<int>& face) {
    std::vector<std::array<int,3>> tris;
    int n = static_cast<int>(face.size());
    if (n == 3) {
        tris.push_back({face[0], face[1], face[2]});
    } else if (n >= 4) {
        for (int i = 1; i < n - 1; ++i) {
            tris.push_back({face[0], face[i], face[i + 1]});
        }
    }
    return tris;
}

// 两个面三角形是否在内部相交（增强共面重叠检测）
bool FaceTrianglesIntersect(const Point& a1, const Point& a2, const Point& a3,
                            const Point& b1, const Point& b2, const Point& b3,
                            double tol) {
    // 先检查是否共面（法线平行）
    Vector3f nA = CrossProduct(a2 - a1, a3 - a1);
    Vector3f nB = CrossProduct(b2 - b1, b3 - b1);
    double nA2 = DotProduct(nA, nA);
    double nB2 = DotProduct(nB, nB);
    if (nA2 < 1e-30 || nB2 < 1e-30) return false; // 退化三角形
    double cosAngle = DotProduct(nA, nB) / (std::sqrt(nA2) * std::sqrt(nB2));
    if (std::abs(cosAngle) > 0.999999) { // 近似共面
        return CoplanarTrianglesOverlap(a1, a2, a3, b1, b2, b3, tol);
    }

    // 非共面，使用 Moller 算法
    if (!MollerTriTri(a1, a2, a3, b1, b2, b3)) return false;

    // 排除边相交（边界接触不算内部相交）
    const Point* a[3] = {&a1, &a2, &a3};
    const Point* b[3] = {&b1, &b2, &b3};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (SegmentsIntersect(*a[i], *a[(i + 1) % 3],
                                  *b[j], *b[(j + 1) % 3], tol)) {
                return false;
            }
        }
    }
    // 重合顶点计数（共享1个或2个顶点视为相邻）
    int nCoincident = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (PointsAreCoincident(*a[i], *b[j], tol)) ++nCoincident;
        }
    }
    return nCoincident != 1 && nCoincident != 2;
}

// 检查所有非相邻面是否相交
bool NoIntersectingFaces(const std::vector<Point>& pts,
                         const std::vector<std::vector<int>>& faces, double tol) {
    std::vector<std::vector<std::array<int,3>>> faceTris;
    faceTris.reserve(faces.size());
    for (const auto& f : faces) {
        faceTris.push_back(TriangulateFace(f));
    }
    for (size_t i = 0; i < faces.size(); ++i) {
        for (size_t j = i + 1; j < faces.size(); ++j) {
            for (const auto& t1 : faceTris[i]) {
                for (const auto& t2 : faceTris[j]) {
                    if (FaceTrianglesIntersect(pts[t1[0]], pts[t1[1]], pts[t1[2]],
                                               pts[t2[0]], pts[t2[1]], pts[t2[2]], tol)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// 2D 多边形凸性（vtkPolygon::IsConvex）
bool IsConvex2D(const std::vector<Point>& pts) {
    int n = static_cast<int>(pts.size());
    if (n <= 3) return true;
    double nx = 0.0, ny = 0.0, nz = 0.0;
    for (int i = 0; i < n; ++i) {
        const Point& a = pts[i];
        const Point& b = pts[(i + 1) % n];
        nx += static_cast<double>(a[1] - b[1]) * static_cast<double>(a[2] + b[2]);
        ny += static_cast<double>(a[2] - b[2]) * static_cast<double>(a[0] + b[0]);
        nz += static_cast<double>(a[0] - b[0]) * static_cast<double>(a[1] + b[1]);
    }
    for (int i = 0; i < n; ++i) {
        const Point& prev = pts[(i - 1 + n) % n];
        const Point& curr = pts[i];
        const Point& next = pts[(i + 1) % n];
        Vector3f cr = CrossProduct(curr - prev, next - curr);
        double dot = nx * static_cast<double>(cr[0]) +
                     ny * static_cast<double>(cr[1]) +
                     nz * static_cast<double>(cr[2]);
        if (dot < 0.0) return false;
    }
    return true;
}

// 3D 多面体凸性（vtkPolyhedron::IsConvex）
bool IsConvex3D(const std::vector<Point>& pts,
                const std::vector<std::vector<int>>& faces) {
    int nPts = static_cast<int>(pts.size());
    for (const auto& face : faces) {
        int nf = static_cast<int>(face.size());
        if (nf < 3) return false;
        double nx = 0.0, ny = 0.0, nz = 0.0;
        for (int i = 0; i < nf; ++i) {
            const Point& a = pts[face[i]];
            const Point& b = pts[face[(i + 1) % nf]];
            nx += static_cast<double>(a[1] - b[1]) * static_cast<double>(a[2] + b[2]);
            ny += static_cast<double>(a[2] - b[2]) * static_cast<double>(a[0] + b[0]);
            nz += static_cast<double>(a[0] - b[0]) * static_cast<double>(a[1] + b[1]);
        }
        double nrm2 = nx * nx + ny * ny + nz * nz;
        if (nrm2 < 1e-30) return false;
        std::vector<bool> onFace(nPts, false);
        for (int idx : face) onFace[idx] = true;
        const Point& fp = pts[face[0]];
        int side = 0;
        for (int i = 0; i < nPts; ++i) {
            if (onFace[i]) continue;
            double dist = nx * (static_cast<double>(pts[i][0]) - fp[0]) +
                          ny * (static_cast<double>(pts[i][1]) - fp[1]) +
                          nz * (static_cast<double>(pts[i][2]) - fp[2]);
            double relTol = 1e-12 * std::sqrt(nrm2);
            if (std::abs(dist) < relTol) continue;
            int s = (dist > 0.0) ? 1 : -1;
            if (side == 0) side = s;
            else if (side != s) return false;
        }
    }
    return true;
}

// 面朝向正确性（所有面法线指向外部）
bool FacesOrientedCorrectly(const std::vector<Point>& pts,
                            const std::vector<std::vector<int>>& faces) {
    double cx = 0.0, cy = 0.0, cz = 0.0;
    for (const auto& p : pts) {
        cx += p[0]; cy += p[1]; cz += p[2];
    }
    double invN = 1.0 / static_cast<double>(pts.size());
    cx *= invN; cy *= invN; cz *= invN;

    for (const auto& face : faces) {
        int nf = static_cast<int>(face.size());
        if (nf < 3) return false;
        std::vector<Point> fp;
        fp.reserve(nf);
        for (int idx : face) fp.push_back(pts[idx]);
        // 检查面自身有效性（无自交边、凸性）
        if (nf >= 4) {
            for (int i = 0; i < nf; ++i) {
                for (int j = i + 2; j < nf; ++j) {
                    if (i == 0 && j == nf - 1) continue;
                    if (SegmentsIntersect(fp[i], fp[(i + 1) % nf],
                                          fp[j], fp[(j + 1) % nf], kTol)) {
                        return false;
                    }
                }
            }
            if (!IsConvex2D(fp)) return false;
        }
        double nx = 0.0, ny = 0.0, nz = 0.0;
        double fx = 0.0, fy = 0.0, fz = 0.0;
        for (int i = 0; i < nf; ++i) {
            const Point& a = pts[face[i]];
            const Point& b = pts[face[(i + 1) % nf]];
            nx += static_cast<double>(a[1] - b[1]) * static_cast<double>(a[2] + b[2]);
            ny += static_cast<double>(a[2] - b[2]) * static_cast<double>(a[0] + b[0]);
            nz += static_cast<double>(a[0] - b[0]) * static_cast<double>(a[1] + b[1]);
            fx += a[0]; fy += a[1]; fz += a[2];
        }
        fx /= nf; fy /= nf; fz /= nf;
        double nlen = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (nlen < 1e-30) return false;
        double ox = fx - cx, oy = fy - cy, oz = fz - cz;
        double olen = std::sqrt(ox * ox + oy * oy + oz * oz);
        if (olen < 1e-30) continue;
        double dot = (nx * ox + ny * oy + nz * oz) / (nlen * olen);
        if (dot < 0.0) return false;
    }
    return true;
}

// ===========================================================================
// 单元级检查函数（每个类型）
// ===========================================================================

unsigned short CheckTriangle(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 3) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    double area2 = TriangleArea2Sq(pts[0], pts[1], pts[2]);
    if (area2 < 1e-30) state |= Validity_Nonconvex; // 共线退化
    return state;
}

unsigned short CheckQuad(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 4) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    const int edges[4][2] = {{0,1},{1,2},{2,3},{3,0}};
    if (!NoIntersectingEdges(pts, edges, 4, kTol))
        state |= Validity_IntersectingEdges;
    if (!IsConvex2D(pts))
        state |= Validity_Nonconvex;
    return state;
}

unsigned short CheckPolygon(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    int n = static_cast<int>(pts.size());
    if (n < 3) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    // 检查边相交（跳过相邻边）
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (j == i + 1 || (i == 0 && j == n - 1)) continue; // 相邻边
            if (SegmentsIntersect(pts[i], pts[(i + 1) % n],
                                  pts[j], pts[(j + 1) % n], kTol)) {
                state |= Validity_IntersectingEdges;
                break;
            }
        }
        if (state & Validity_IntersectingEdges) break;
    }
    if (!IsConvex2D(pts))
        state |= Validity_Nonconvex;
    return state;
}

unsigned short CheckTetra(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 4) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    double vol = TetraSignedVolume(pts[0], pts[1], pts[2], pts[3]);
    if (std::abs(vol) < 1e-30) state |= Validity_Nonconvex; // 共面或负体积（但负体积可由朝向检测捕获）
    if (!NoIntersectingEdges(pts, kTetraEdges, 6, kTol))
        state |= Validity_IntersectingEdges;
    if (!NoIntersectingFaces(pts, kTetraFaces, kTol))
        state |= Validity_IntersectingFaces;
    return state;
}

unsigned short CheckHex(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 8) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    if (!NoIntersectingEdges(pts, kHexEdges, 12, kTol))
        state |= Validity_IntersectingEdges;
    if (!NoIntersectingFaces(pts, kHexFaces, kTol))
        state |= Validity_IntersectingFaces;
    if (!IsConvex3D(pts, kHexFaces))
        state |= Validity_Nonconvex;
    if (!FacesOrientedCorrectly(pts, kHexFaces))
        state |= Validity_FacesAreOrientedIncorrectly;
    return state;
}

unsigned short CheckWedge(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 6) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    if (!NoIntersectingEdges(pts, kWedgeEdges, 9, kTol))
        state |= Validity_IntersectingEdges;
    if (!NoIntersectingFaces(pts, kWedgeFaces, kTol))
        state |= Validity_IntersectingFaces;
    if (!IsConvex3D(pts, kWedgeFaces))
        state |= Validity_Nonconvex;
    if (!FacesOrientedCorrectly(pts, kWedgeFaces))
        state |= Validity_FacesAreOrientedIncorrectly;
    return state;
}

unsigned short CheckPyramid(const std::vector<Point>& pts) {
    unsigned short state = Validity_Valid;
    if (pts.size() != 5) { state |= Validity_WrongNumberOfPoints; return state; }
    if (HasDuplicatePoints(pts, kTol)) state |= Validity_Nonconvex;
    if (!NoIntersectingEdges(pts, kPyramidEdges, 8, kTol))
        state |= Validity_IntersectingEdges;
    if (!NoIntersectingFaces(pts, kPyramidFaces, kTol))
        state |= Validity_IntersectingFaces;
    // 金字塔也检查凸性和朝向（VTK 未检查，但为了更严格我们添加）
    if (!IsConvex3D(pts, kPyramidFaces))
        state |= Validity_Nonconvex;
    if (!FacesOrientedCorrectly(pts, kPyramidFaces))
        state |= Validity_FacesAreOrientedIncorrectly;
    return state;
}

// 返回单元线性点数
int GetCornerPointCount(int cellType) {
    switch (cellType) {
        case IG_VERTEX: return 1;
        case IG_LINE:
        case IG_QUADRATIC_EDGE: return 2;
        case IG_TRIANGLE:
        case IG_QUADRATIC_TRIANGLE:
        case IG_BIQUADRATIC_TRIANGLE: return 3;
        case IG_QUAD:
        case IG_QUADRATIC_QUAD:
        case IG_BIQUADRATIC_QUAD:
        case IG_QUADRATIC_LINEAR_QUAD: return 4;
        case IG_TETRA:
        case IG_QUADRATIC_TETRA: return 4;
        case IG_PYRAMID:
        case IG_QUADRATIC_PYRAMID:
        case IG_TRIQUADRATIC_PYRAMID: return 5;
        case IG_PRISM:
        case IG_QUADRATIC_PRISM:
        case IG_QUADRATIC_LINEAR_WEDGE:
        case IG_BIQUADRATIC_QUADRATIC_WEDGE: return 6;
        case IG_HEXAHEDRON:
        case IG_QUADRATIC_HEXAHEDRON:
        case IG_TRIQUADRATIC_HEXAHEDRON:
        case IG_BIQUADRATIC_QUADRATIC_HEXAHEDRON: return 8;
        default: return -1;
    }
}

} // anonymous namespace

// ===========================================================================
// ValidateCellsFilter 实现
// ===========================================================================

ValidateCellsFilter::ValidateCellsFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}

bool ValidateCellsFilter::Execute() {
    DataObject::Pointer input = GetInput(0);
    if (input == nullptr) return false;

    m_InvalidCellIds.clear();

    if (auto mesh = DynamicCast<UnstructuredMesh>(input)) {
        const IGsize nCells = mesh->GetNumberOfCells();
        IntArray::Pointer stateArray = IntArray::New();
        stateArray->SetDimension(1);
        stateArray->Resize(static_cast<igIndex>(nCells));
        stateArray->SetName("ValidityState");

        IdArray::Pointer ids = IdArray::New();
        std::vector<Point> points;

        for (IGsize cellId = 0; cellId < nCells; ++cellId) {
            ids->Reset();
            mesh->GetCellPointIds(cellId, ids);
            int nPts = ids->GetNumberOfIds();
            int cellType = mesh->GetCellType(cellId);

            int cornerCount = GetCornerPointCount(cellType);
            points.clear();
            if (cornerCount > 0 && nPts >= cornerCount) {
                points.reserve(cornerCount);
                for (int i = 0; i < cornerCount; ++i)
                    points.push_back(mesh->GetPoint(ids->GetId(i)));
            } else {
                points.reserve(nPts);
                for (int i = 0; i < nPts; ++i)
                    points.push_back(mesh->GetPoint(ids->GetId(i)));
            }

            unsigned short state = Validity_Valid;
            switch (cellType) {
                case IG_EMPTY_CELL:
                    state = Validity_WrongNumberOfPoints;
                    break;
                case IG_VERTEX:
                    state = (points.size() == 1) ? Validity_Valid : Validity_WrongNumberOfPoints;
                    break;
                case IG_LINE:
                case IG_QUADRATIC_EDGE:
                    state = (points.size() == 2) ? Validity_Valid : Validity_WrongNumberOfPoints;
                    break;
                case IG_TRIANGLE:
                case IG_QUADRATIC_TRIANGLE:
                case IG_BIQUADRATIC_TRIANGLE:
                    state = CheckTriangle(points);
                    break;
                case IG_QUAD:
                case IG_QUADRATIC_QUAD:
                case IG_BIQUADRATIC_QUAD:
                case IG_QUADRATIC_LINEAR_QUAD:
                    state = CheckQuad(points);
                    break;
                case IG_POLYGON:
                case IG_FACE:
                case IG_QUADRATIC_POLYGON:
                    state = CheckPolygon(points);
                    break;
                case IG_TETRA:
                case IG_QUADRATIC_TETRA:
                    state = CheckTetra(points);
                    break;
                case IG_HEXAHEDRON:
                case IG_QUADRATIC_HEXAHEDRON:
                case IG_TRIQUADRATIC_HEXAHEDRON:
                case IG_BIQUADRATIC_QUADRATIC_HEXAHEDRON:
                    state = CheckHex(points);
                    break;
                case IG_PYRAMID:
                case IG_QUADRATIC_PYRAMID:
                case IG_TRIQUADRATIC_PYRAMID:
                    state = CheckPyramid(points);
                    break;
                case IG_PRISM:
                case IG_QUADRATIC_PRISM:
                case IG_QUADRATIC_LINEAR_WEDGE:
                case IG_BIQUADRATIC_QUADRATIC_WEDGE:
                    state = CheckWedge(points);
                    break;
                default:
                    state = Validity_Valid;
                    break;
            }

            stateArray->SetValue(static_cast<igIndex>(cellId), static_cast<int>(state));
            if (state != Validity_Valid) {
                m_InvalidCellIds.push_back(static_cast<igIndex>(cellId));
            }
        }

        stateArray->Modified();
        if (mesh->GetAttributeSet()) {
            mesh->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_CELL, stateArray);
            mesh->GetAttributeSet()->Modified();
        }
        mesh->Modified();

    } else if (auto mesh = DynamicCast<SurfaceMesh>(input)) {
        const IGsize nFaces = mesh->GetNumberOfFaces();
        IntArray::Pointer stateArray = IntArray::New();
        stateArray->SetDimension(1);
        stateArray->Resize(static_cast<igIndex>(nFaces));
        stateArray->SetName("ValidityState");

        for (IGsize faceId = 0; faceId < nFaces; ++faceId) {
            igIndex pointIds[3]{};
            mesh->GetFacePointIds(faceId, pointIds);
            std::vector<Point> pts = {
                mesh->GetPoint(pointIds[0]),
                mesh->GetPoint(pointIds[1]),
                mesh->GetPoint(pointIds[2])
            };
            unsigned short state = CheckTriangle(pts);
            stateArray->SetValue(static_cast<igIndex>(faceId), static_cast<int>(state));
            if (state != Validity_Valid) {
                m_InvalidCellIds.push_back(static_cast<igIndex>(faceId));
            }
        }

        stateArray->Modified();
        if (mesh->GetAttributeSet()) {
            mesh->GetAttributeSet()->AddAttribute(IG_SCALAR, IG_CELL, stateArray);
            mesh->GetAttributeSet()->Modified();
        }
        mesh->Modified();
    } else {
        return false;
    }

    SetOutput(0, input);

    // 高亮无效单元
    if (m_Model != nullptr && !m_InvalidCellIds.empty()) {
        auto selection = m_Model->GetSelection();
        if (selection != nullptr) {
            selection->SelectionCallBackEvent(IG_CELL, m_InvalidCellIds,
                                              Selection::Operate::Add);
        }
    }

    return true;
}

IGAME_NAMESPACE_END