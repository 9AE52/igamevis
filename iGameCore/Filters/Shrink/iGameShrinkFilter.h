#ifndef iGameShrinkFilter_h
#define iGameShrinkFilter_h

#include "iGameFilter.h"
#include "iGameSurfaceMesh.h"
#include "iGameVolumeMesh.h"
#include "iGameUnstructuredMesh.h"

IGAME_NAMESPACE_BEGIN
class ShrinkFilter : public Filter {
public:
	I_OBJECT(ShrinkFilter);
	static Pointer New() { return new ShrinkFilter; }

	// 收缩比例：0~1 之间。
	// 1.0 = 原地不动；0.5 = 每个顶点缩到“原位置和质心正中间”（默认）；0.0 = 全部缩成质心一点。
	void SetShrinkFactor(double factor);
	double GetShrinkFactor() const;

	bool Execute() override;

protected:
	ShrinkFilter();
	~ShrinkFilter() override = default;

private:
	double m_ShrinkFactor{0.5};
};
IGAME_NAMESPACE_END
#endif
