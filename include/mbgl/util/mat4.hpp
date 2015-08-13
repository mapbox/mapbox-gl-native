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

#ifndef MBGL_UTIL_MAT4
#define MBGL_UTIL_MAT4

#include <array>

namespace mbgl {

typedef std::array<float, 16> mat4;

namespace matrix {

void identity(mat4& out);
void ortho(mat4& out, float left, float right, float bottom, float top, float near, float far);
void perspective(mat4& out, float fovy, float aspect, float near, float far);
void copy(mat4& out, const mat4& a);
void translate(mat4& out, const mat4& a, float x, float y, float z);
void rotate_x(mat4& out, const mat4& a, float rad);
void rotate_z(mat4& out, const mat4& a, float rad);
void scale(mat4& out, const mat4& a, float x, float y, float z);
void multiply(mat4& out, const mat4& a, const mat4& b);

}
}

#endif
