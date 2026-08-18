#ifndef iGameGhostCellFilter_h
#define iGameGhostCellFilter_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameUnstructuredMesh.h"
#include "iGameVolumeMesh.h"

#include <string>
#include <vector>

IGAME_NAMESPACE_BEGIN
class GhostCellFilter : public Filter {
public:
    I_OBJECT(GhostCellFilter);
    static Pointer New() { return new GhostCellFilter; }

    // 设置"点 ghost 标记数组"的名字（默认 GhostPoints）。
    // 该数组必须是挂在"点"上的标量数组，值非 0 表示该点是 ghost 点。
    void SetPointGhostArrayName(const std::string& name) { m_PointGhostArrayName = name; }

    bool Execute() override;

protected:
    GhostCellFilter();
    ~GhostCellFilter() override = default;

private:
    // 1. 从输入网格的属性集里读取"点 ghost 标记数组"（可选，找不到也没关系）
    bool LoadPointGhostArray(DataObject::Pointer input, std::vector<char>& pointGhosts);

    // 2. 根据点 ghost 标记计算每个单元是不是 ghost 单元（核心规则写在这里）
    bool ComputeCellGhosts(DataObject::Pointer input, const std::vector<char>& pointGhosts, bool hasPointGhosts,
                           std::vector<char>& cellGhosts);

    // 3. 把标记数组挂到输入网格的"单元"属性上
    bool AttachCellGhostArray(DataObject::Pointer input, const std::vector<char>& cellGhosts);

    std::string m_PointGhostArrayName{"GhostPoints"};
};
IGAME_NAMESPACE_END
#endif