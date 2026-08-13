//
// Created by Sumzeek on 4/12/2024.
//

#include "type_vec3.h"

namespace igm
{
template<typename T>
vec<3, T> min(const vec<3, T>& v1, const vec<3, T>& v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z)};
}

template<typename T>
vec<3, T> max(const vec<3, T>& v1, const vec<3, T>& v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z)};
}

template<typename T>
T dot(const vec<3, T>& v1, const vec<3, T>& v2) {
    return v1.dot(v2);
}

template<typename T>
vec<3, T> cross(const vec<3, T>& v1, const vec<3, T>& v2) {
    return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x};
}
} // namespace igm