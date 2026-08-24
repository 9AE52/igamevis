#pragma once
#ifndef EOIGAME_IGAMECORE_PROCESSGET_IGAMEGENERATEPROCESSIDSFILTER_H
#define EOIGAME_IGAMECORE_PROCESSGET_IGAMEGENERATEPROCESSIDSFILTER_H

#include <iGameDataObject.h>
#include <iGameFilter.h>
#include <iGameModel.h>
#include <iGamePoints.h>
#include <iGameUnstructuredMesh.h>

IGAME_NAMESPACE_BEGIN
class GenerateProcessIdsFilter : public Filter {
public:
    I_OBJECT(GenerateProcessIdsFilter)

    static Pointer New() { return new GenerateProcessIdsFilter; }

    bool Execute() override;

    void SetGeneratePointData(bool b) { m_GeneratePointData = b; }
    bool GetGeneratePointData() const { return m_GeneratePointData; }

    void SetGenerateCellData(bool b) { m_GenerateCellData = b; }
    bool GetGenerateCellData() const { return m_GenerateCellData; }

    void SetProcessId(int pid) { m_ProcessId = pid; }
    int GetProcessId() const { return m_ProcessId; }

protected:
    GenerateProcessIdsFilter();
    ~GenerateProcessIdsFilter() override = default;

    virtual long long GetPointProcessId(IGsize index);
    virtual long long GetCellProcessId(IGsize index);

    LongLongArray::Pointer m_PointProcessIdArray{nullptr};
    LongLongArray::Pointer m_CellProcessIdArray{nullptr};

    int m_ProcessId{0};
    bool m_GeneratePointData{true};
    bool m_GenerateCellData{false};
};
IGAME_NAMESPACE_END
#endif
