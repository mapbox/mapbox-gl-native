// This is an incomplete port of http://glmatrix.net/
//
// Copyright (c) 2013 Brandon Jones, Colin MacKenzie IV
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not claim
//    that you wrote the original software. If you use this software in a
//    product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

#pragma once

#include <array>
#include <cmath>

namespace mbgl {

using vec3 = std::array<double, 3>;
using vec3f = std::array<float, 3>;
using vec3i = std::array<int, 3>;
using mat3 = std::array<double, 9>;

inline vec3 vec3Cross(const vec3& a, const vec3& b) {
    return vec3{a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

inline double vec3Dot(const vec3& a, const vec3& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline double vec3LengthSq(const vec3& a) {
    return vec3Dot(a, a);
}

inline double vec3Length(const vec3& a) {
    return std::sqrt(vec3LengthSq(a));
}

inline vec3 vec3Scale(const vec3& a, double s) {
    return vec3{a[0] * s, a[1] * s, a[2] * s};
}

inline vec3 vec3Normalize(const vec3& a) {
    return vec3Scale(a, 1.0 / vec3Length(a));
}

inline vec3 vec3Sub(const vec3& a, const vec3& b) {
    return vec3{a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

namespace matrix {

void identity(mat3& out);
void translate(mat3& out, const mat3& a, double x, double y);
void rotate(mat3& out, const mat3& a, double rad);
void scale(mat3& out, const mat3& a, double x, double y);

void transformMat3f(vec3f& out, const vec3f& a, const mat3& m);

} // namespace matrix
} // namespace mbgl
