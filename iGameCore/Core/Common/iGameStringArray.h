#ifndef iGameStringArray_h
#define iGameStringArray_h

#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
class StringArray : public ElementArray<std::string> {
public:
    I_OBJECT(StringArray);
    static Pointer New() { return new StringArray; }

protected:
    StringArray() = default;
    ~StringArray() override = default;
};
IGAME_NAMESPACE_END
#endif