#ifndef iGameIdArray_h
#define iGameIdArray_h

#include "iGameElementArray.h"

IGAME_NAMESPACE_BEGIN
class IdArray : public ElementArray<igIndex> {
public:
    I_OBJECT(IdArray);
    static Pointer New() { return new IdArray; }
    using SuperClass = ElementArray<igIndex>;
    using Reference = typename SuperClass::Reference;
    using ConstReference = typename SuperClass::ConstReference;

    Reference GetId(const IGsize _Pos) {
        return this->SuperClass::GetElement(_Pos);
    }
    ConstReference GetId(const IGsize _Pos) const {
        return this->SuperClass::GetElement(_Pos);
    }

    void SetId(const IGsize _Pos, igIndex&& _Id) {
        this->SuperClass::SetElement(_Pos, _Id);
    }
    void SetId(const IGsize _Pos, const igIndex& _Id) {
        this->SuperClass::SetElement(_Pos, _Id);
    }

    void AddId(igIndex&& _Id) {
        this->SuperClass::AddElement(_Id);
    }
    void AddId(const igIndex& _Id) {
        this->SuperClass::AddElement(_Id);
    }

    IGsize SearchId(igIndex id) const
    {
        IGsize i;
        for (i = 0; i < this->GetNumberOfIds(); i++)
        {
            if (this->GetId(i) == id)
            {
                return i;
            }
        }
        return (-1);
    }

    void SetNumberOfIds(const IGsize _Newsize) {
        this->SuperClass::Resize(_Newsize);
    }

    IGsize GetNumberOfIds() const {
        return this->SuperClass::GetNumberOfElements();
    }

    Reference GetElement(const IGsize _Pos) = delete;
    ConstReference GetElement(const IGsize _Pos) const = delete;
    void SetElement(const IGsize _Pos, igIndex&& _Element) = delete;
    void SetElement(const IGsize _Pos, const igIndex& _Element) = delete;
    void AddElement(igIndex&& _Element) = delete;
    void AddElement(const igIndex& _Element) = delete;
    IGsize GetNumberOfElements() const = delete;

    IGsize GetRealMemorySize() {
        return  this->GetNumberOfIds() * sizeof(igIndex);
    }
protected:
    IdArray() = default;
    ~IdArray() override = default;
};
IGAME_NAMESPACE_END
#endif