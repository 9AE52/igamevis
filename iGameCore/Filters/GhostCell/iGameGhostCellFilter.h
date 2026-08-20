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

    void SetPointGhostArrayName(const std::string& name) { m_PointGhostArrayName = name; }

    bool Execute() override;

protected:
    GhostCellFilter();
    ~GhostCellFilter() override = default;

private:
    // 从输入网格的属性集里读取点 ghost 标记数组
    bool LoadPointGhostArray(DataObject::Pointer input, std::vector<char>& pointGhosts);

    //根据点 ghost 标记计算每个单元是不是 ghost 单元
    bool ComputeCellGhosts(DataObject::Pointer input, const std::vector<char>& pointGhosts, bool hasPointGhosts,
                           std::vector<char>& cellGhosts);

    //  把标记数组挂到输入网格的单元属性上
    bool AttachCellGhostArray(DataObject::Pointer input, const std::vector<char>& cellGhosts);

    std::string m_PointGhostArrayName{"GhostPoints"};
};
IGAME_NAMESPACE_END
#endif