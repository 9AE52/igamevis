#ifndef iGameHashMap_h
#define iGameHashMap_h

#include "iGameObject.h"
#include <unordered_map>

IGAME_NAMESPACE_BEGIN
template<typename TKey, typename TValue>
class HashMap : public Object, private std::unordered_map<TKey, TValue> {
public:
    I_OBJECT(HashMap);
    static Pointer New() { return new HashMap; }
    using MapType = std::unordered_map<TKey, TValue>;
    using Iterator = typename MapType::iterator;
    using ConstIterator = typename MapType::const_iterator;
    using Reference = typename MapType::reference;
    using ConstReference = typename MapType::const_reference;
    using SizeType = typename MapType::size_type;

    // Free all memory and initialize the map
    void Initialize() {
        this->MapType::clear();
    }

    void Insert(const TKey& key, const TValue& value) {
        (*this)[key] = value;
    }

    TValue& Get(const TKey& key) {
        return this->MapType::at(key);
    }

    const TValue& Get(const TKey& key) const {
        return this->MapType::at(key);
    }

    Iterator FindKey(const TKey& key) {
        return this->MapType::find(key);
    }

    ConstIterator FindKey(const TKey& key) const {
        return this->MapType::find(key);
    }

    // Check if the key exists
    bool ContainsKey(const TKey& key) const {
        return this->MapType::find(key) != this->MapType::end();
    }

    // Remove element by key
    bool Remove(const TKey& key) {
        return this->MapType::erase(key) > 0;
    }

    // Get the number of elements
    SizeType Size() const {
        return this->MapType::size();
    }

    // Clear all
    void Clear() {
        this->MapType::clear();
    }

    // Get iterator to the beginning
    Iterator Begin() {
        return this->MapType::begin();
    }

    ConstIterator Begin() const {
        return this->MapType::begin();
    }

    // Get iterator to the end
    Iterator End() {
        return this->MapType::end();
    }

    ConstIterator End() const {
        return this->MapType::end();
    }

protected:
    HashMap() {}
    ~HashMap() override = default;
};
IGAME_NAMESPACE_END
#endif