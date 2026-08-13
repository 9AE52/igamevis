//
// Created by Sumzeek on 4/12/2024.
//

#include "type_vec2.h"

namespace igm
{
template<typename T>
vec<2, T> min(const vec<2, T>& v1, const vec<2, T>& v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
}

template<typename T>
vec<2, T> max(const vec<2, T>& v1, const vec<2, T>& v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
}

template<typename T>
T dot(const vec<2, T>& v1, const vec<2, T>& v2) {
    return v1.dot(v2);
}
} // namespace igm