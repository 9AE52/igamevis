#pragma once

#include "iGameFilter.h"
#include "iGameUnstructuredMesh.h"

IGAME_NAMESPACE_BEGIN

class RemoveGhostInformationFilter : public Filter {
public:
    I_OBJECT(RemoveGhostInformationFilter);

    static Pointer New() { return new RemoveGhostInformationFilter; }

    bool Execute() override;

protected:
    RemoveGhostInformationFilter();
    ~RemoveGhostInformationFilter() override = default;
};

IGAME_NAMESPACE_END


