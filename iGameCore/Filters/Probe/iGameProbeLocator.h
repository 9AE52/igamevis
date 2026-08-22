// ============================================================================
// ProbeLocator — ProbeFilter 的定位/插值几何函数集合
//
// 约定（按需求）：
//   - 面单元判断函数只接受三角形，非三角形直接返回 false（不做扇拆）；
//   - 体单元判断函数只接受四面体，非四面体直接返回 false（不做扇拆）；
//   - 命中时输出 simplex（三角形/四面体）的重心坐标，调用方据此对点属性
//     做线性插值；
//   - 几何容差集中在本文件顶部常量，便于调整。
// ============================================================================
#pragma once
#include <iGameCell.h>
#include <iGamePoints.h>
#include <iGameVector.h>

IGAME_NAMESPACE_BEGIN

// ---- 几何容差（可调）----
// 重心坐标容差（无量纲）：允许边界上因浮点误差出现的小负值
inline constexpr double kProbeBarycentricEps = 1e-6;
// 数值下限：避免退化为 0 的单元被误判
inline constexpr double kProbeNumericalEps = 1e-12;

// 单次命中的 simplex（三角形/四面体）信息
struct ProbeSimplexHit {
    bool found{false};
    int numVertices{0};                     // 3 = 三角形, 4 = 四面体
    int localVertIds[4] = {-1, -1, -1, -1}; // 单元内顶点序号
    double barycentric[4] = {0.0, 0.0, 0.0, 0.0};  // 重心坐标
};

// 面单元判断：非三角形直接返回 false；命中时填充 hit
bool ProbeLocateInFace(const Cell* cell, const Point& q, ProbeSimplexHit& hit);

// 体单元判断：非四面体直接返回 false；命中时填充 hit
bool ProbeLocateInVolume(const Cell* cell, const Point& q, ProbeSimplexHit& hit);

// 点是否在三角形上（含边界）；命中时给出重心坐标
bool ProbePointInTriangle(const Point& q, const Point& a, const Point& b,
                          const Point& c, double lambda[3]);

// 点是否在四面体内（含边界）；命中时给出重心坐标
bool ProbePointInTetra(const Point& q, const Point& a, const Point& b,
                       const Point& c, const Point& d, double lambda[4]);

IGAME_NAMESPACE_END
