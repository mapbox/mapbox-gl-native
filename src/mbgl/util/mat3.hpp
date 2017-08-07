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

namespace mbgl {

using vec3 = std::array<double, 3>;
using vec3f = std::array<float, 3>;
using mat3 = std::array<double, 9>;

namespace matrix {

void identity(mat3& out);
void translate(mat3& out, const mat3& a, double x, double y);
void rotate(mat3& out, const mat3& a, double rad);
void scale(mat3& out, const mat3& a, double x, double y);

void transformMat3f(vec3f& out, const vec3f& a, const mat3& m);

} // namespace matrix
} // namespace mbgl
