// ============================================================================
// ProbeFilter  — 在指定位置探测数据（点定位 + 插值）
// 由 Script/igame_new_filter.py 生成骨架后完成实现
//
// 数据流: data_object -> point_set（探测结果）   输入端口 1 / 输出端口 0
//
// 用法:
//   auto f = ProbeFilter::New();
//   f->SetInput(obj);                  // 任意带点坐标/点属性的数据对象
//   f->SetProbePoints(probePoints);    // 探测点（最通用的 PointSet）
//   f->SetNeighborCount(8);            // 可选，K 近邻插值近邻数
//   f->Execute();
//   auto result = f->GetResult();      // 结果 PointSet：探测点 + 插值属性
//
// 与 Selection 系列（GetClosestPointsInLineFilter 等）保持一致：
// 算法在 Execute() 中执行，结果通过 GetResult() 等查询函数读取。
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

    // ---- 探测点输入（最通用的点集数据结构）----
    void SetProbePoints(PointSet::Pointer points);
    void SetProbePoints(Points::Pointer points);
    IGsize AddProbePoint(const Point& point);
    IGsize AddProbePoint(float x, float y, float z);
    IGsize GetNumberOfProbePoints() const;

    // ---- 结果查询（Execute() 成功之后调用）----
    PointSet::Pointer GetResult();
    DoubleArray::Pointer GetProbeDistances();
    IntArray::Pointer GetLocatedPointIds();

    // ---- 算法参数 ----
    void SetNeighborCount(int value) { m_NeighborCount = value; }
    int GetNeighborCount() const { return m_NeighborCount; }

protected:
    ProbeFilter();
    ~ProbeFilter() override = default;

private:
    /* Input */
    PointSet::Pointer m_ProbePoints{};

    /* Output */
    PointSet::Pointer m_Result{};
    DoubleArray::Pointer m_ProbeDistances{};
    IntArray::Pointer m_LocatedPointIds{};

    /* Algorithm parameters */
    int m_NeighborCount{8};
};
IGAME_NAMESPACE_END
