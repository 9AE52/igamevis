#ifndef iGamePropertyTree_h
#define iGamePropertyTree_h

#include "iGameVariant.h"
#include "iGameElementArray.h"
#include "iGameHashMap.h"

IGAME_NAMESPACE_BEGIN
class Property : public Object {
public:
    I_OBJECT(Property);
    static Pointer New(Variant::Type type, const std::string& name = std::string("")) { return new Property(type, name); }

    void SetValue(const Variant& value) {
        this->value = value;
    }

    const Variant& GetValue() const {
        return value;
    }

    template<class _Ty>
    _Ty Get() const {
        if (!std::holds_alternative<std::monostate>(value)) {
            return std::get<_Ty>(value);
        }
        return _Ty{};
    }

    IGenum GetType() const {
        return type;
    }

    const std::string& GetName() const {
        return name;
    }

    void SetEnabled(bool flag) {
        enabled = flag;
    }

    bool IsEnabled() const {
        return enabled;
    }

    void AddSubProperty(Property::Pointer prop) {
        if (subProperties == nullptr) {
            Initialize();
        }
        subProperties->AddElement(prop);
        subPropertyMap->Insert(prop->GetName(), subProperties->Size() - 1);
    }

    Property::Pointer AddSubProperty(Variant::Type type, const std::string& name = std::string("")) {
        if (subProperties == nullptr) {
            Initialize();
        }
        auto property = Property::New(type, name);
        subProperties->AddElement(property);
        subPropertyMap->Insert(name, subProperties->Size() - 1);
        return property;
    }

    Property::Pointer GetSubProperty(const std::string& name) const {
        if (subPropertyMap == nullptr) {
            return nullptr;
        }
        auto it = subPropertyMap->FindKey(name);
        if (it != subPropertyMap->End()) {
            return subProperties->GetElement(it->second);
        }
        return nullptr;
    }

    Property::Pointer GetSubProperty(const int index) const {
        if (subPropertyMap == nullptr) {
            return nullptr;
        }
        if (index >= 0 && index < subProperties->Size()) {
            return subProperties->GetElement(index);
        }
        return nullptr;
    }

    IGsize Size() const {
        if (subProperties == nullptr) {
            return 0;
        }
        return subProperties->Size();
    }

private:
    Property(Variant::Type type, const std::string& name) : type(type), name(name) {}

    void Initialize() {
        subProperties = ElementArray<Property::Pointer>::New();
        subPropertyMap = HashMap<std::string, int>::New();
    }

    std::string name;
    IGenum type{ Variant::Invalid };
    Variant value{};
    bool enabled{ false };

    ElementArray<Property::Pointer>::Pointer subProperties;
    HashMap<std::string, int>::Pointer subPropertyMap;
};

class PropertyTree : public Object {
public:
    I_OBJECT(PropertyTree);
    static Pointer New() { return new PropertyTree; }

    void AddProperty(Property::Pointer prop) {
        properties->AddElement(prop);
        propertyMap->Insert(prop->GetName(), properties->Size() - 1);
    }

    Property::Pointer AddProperty(Variant::Type type, const std::string& name = std::string("")) {
        auto property = Property::New(type, name);
        properties->AddElement(property);
        propertyMap->Insert(name, properties->Size() - 1);
        return property;
    }

    Property::Pointer GetProperty(const std::string& key) const {
        auto it = propertyMap->FindKey(key);
        if (it != propertyMap->End()) {
            return properties->GetElement(it->second);
        }
        return nullptr;
    }

    Property::Pointer GetProperty(const int index) const {
        if (index >= 0 && index < properties->Size()) {
            return properties->GetElement(index);
        }
        return nullptr;
    }

    bool RemoveProperty(const std::string& name) {
        auto it = propertyMap->FindKey(name);
        if (it != propertyMap->End()) {
            int index = it->second;
            // Remove from the vector
            properties->Remove(properties->Begin() + index);
            // Update the map to reflect changes
            propertyMap->Clear();
            for (int i = 0; i < properties->Size(); ++i) {
                propertyMap->Insert(properties->GetElement(i)->GetName(), i);
            }
            return true;
        }
        return false;
    }

    IGsize Size() const {
        return properties->Size();
    }

protected:
    PropertyTree() {
        properties = ElementArray<Property::Pointer>::New();
        propertyMap = HashMap<std::string, int>::New();
    }
    ~PropertyTree() override {}

    ElementArray<Property::Pointer>::Pointer properties;
    HashMap<std::string, int>::Pointer propertyMap;
};
IGAME_NAMESPACE_END
#endif