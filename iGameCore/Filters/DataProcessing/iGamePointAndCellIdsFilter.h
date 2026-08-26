#ifndef iGamePointAndCellIdsFilter_h
#define iGamePointAndCellIdsFilter_h

#include "iGameFilter.h"

#include <string>

IGAME_NAMESPACE_BEGIN

class PointAndCellIdsFilter : public Filter {
public:
    I_OBJECT(PointAndCellIdsFilter);

    static Pointer New() { return new PointAndCellIdsFilter; }

    bool Execute() override;

    // 是否生成点 ID，默认开启
    void SetGeneratePointIds(bool enable) { m_GeneratePointIds = enable; }

    bool GetGeneratePointIds() const { return m_GeneratePointIds; }

    // 是否生成单元 ID，默认开启
    void SetGenerateCellIds(bool enable) { m_GenerateCellIds = enable; }

    bool GetGenerateCellIds() const { return m_GenerateCellIds; }

    // 点 ID 数组名称，默认与 DIME 一致为 PointIds
    void SetPointIdsArrayName(const std::string& name) { m_PointIdsArrayName = name; }

    const std::string& GetPointIdsArrayName() const { return m_PointIdsArrayName; }

    // 单元 ID 数组名称，默认与 DIME 一致为 CellIds
    void SetCellIdsArrayName(const std::string& name) { m_CellIdsArrayName = name; }

    const std::string& GetCellIdsArrayName() const { return m_CellIdsArrayName; }

protected:
    PointAndCellIdsFilter();
    ~PointAndCellIdsFilter() override = default;

private:
    bool m_GeneratePointIds{true};
    bool m_GenerateCellIds{true};

    std::string m_PointIdsArrayName{"PointIds"};
    std::string m_CellIdsArrayName{"CellIds"};
};

IGAME_NAMESPACE_END

#endif