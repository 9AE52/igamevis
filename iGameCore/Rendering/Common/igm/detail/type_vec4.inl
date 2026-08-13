//
// Created by Sumzeek on 4/12/2024.
//

#include "type_vec4.h"

namespace igm
{
template<typename T>
vec<4, T> min(const vec<4, T>& v1, const vec<4, T>& v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z),
            std::max(v1.w, v2.w)};
}

template<typename T>
vec<4, T> max(const vec<4, T>& v1, const vec<4, T>& v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z),
            std::max(v1.w, v2.w)};
}

template<typename T>
T dot(const vec<4, T>& v1, const vec<4, T>& v2) {
    return v1.dot(v2);
}
} // namespace igm