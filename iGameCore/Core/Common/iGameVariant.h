#ifndef iGameVariant_h
#define iGameVariant_h

#include <variant>

#include "iGameObject.h"
#include "iGameStringArray.h"

IGAME_NAMESPACE_BEGIN
using StdVariant = std::variant<std::monostate, unsigned char, char, unsigned short, short,
    unsigned int, int, unsigned long long, long long, float, double, bool, 
    std::string, StringArray::Pointer>;

class Variant : public StdVariant {
public:
    enum Type {
        Invalid = 0,
        Bool,
        Char,
        UChar,
        Short,
        UShort,
        Int,
        UInt,
        Float,
        LongLong,
        ULongLong,
        Double,
        String,
        StringList,
        //Date,
        //Time,
        //DateTime,
    };

    Variant(){}

    template <typename T>
    Variant(T&& value) : StdVariant(std::forward<T>(value)) {}
};
IGAME_NAMESPACE_END
#endif