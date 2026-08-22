// ============================================================================
// ProbeFilter — 单点探测（单元定位 + 线性插值；未命中回退 K 近邻 IDW）
//
// 数据流: data_object -> point_set（探测结果，1 个点） 输入端口 1 / 输出端口 0
//
// 用法:
//   auto f = ProbeFilter::New();
//   f->SetInput(obj);                  // 带点坐标/点属性（可选带单元）的数据对象
//   f->SetProbePoint(x, y, z);         // 探测点（每次只探测一个点）
//   f->SetNeighborCount(8);            // IDW 回退的近邻数，默认 8
//   f->Execute();
//   auto result = f->GetResult();      // 结果 PointSet：探测点 + 插值属性
//
// 算法:
//   1. 遍历所有单元：面单元只接受三角形、体单元只接受四面体（见
//      iGameProbeLocator），命中后按重心坐标对全部点属性做线性插值。
//   2. 全部未命中（或输入无单元）时，遍历全部数据点用最大堆取最近 k 个点
//      做反距离加权插值（IDW）。
// ============================================================================
#pragma once
#include <iGameDataObject.h>
#include <iGameFilter.h>
#include <iGamePointSet.h>
#include <iGamePoints.h>

IGAME_NAMESPACE_BEGIN
class ProbeFilter : public Filter {
public:
    I_OBJECT(ProbeFilter);
    static Pointer New() { return new ProbeFilter; }

    bool Execute() override;

    // ---- 探测点输入（单点）----
    void SetProbePoint(const Point& point);
    void SetProbePoint(float x, float y, float z);

    // ---- 结果查询（Execute() 成功之后调用）----
    PointSet::Pointer GetResult();
    IntArray::Pointer GetProbeMethods();

    // ---- 算法参数 ----
    void SetNeighborCount(int value) { m_NeighborCount = value; }
    int GetNeighborCount() const { return m_NeighborCount; }

protected:
    ProbeFilter();
    ~ProbeFilter() override = default;

private:
    /* Input */
    Point m_ProbePoint{};
    bool m_HasProbePoint{false};

    /* Output */
    PointSet::Pointer m_Result{};
    IntArray::Pointer m_ProbeMethods{};

    /* Algorithm parameters */
    int m_NeighborCount{8};
};
IGAME_NAMESPACE_END
