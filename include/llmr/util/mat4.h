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

#ifndef llmr_util_mat4_
#define llmr_util_mat4_

#ifdef __cplusplus
extern "C" {
#endif

void mat4_identity(float out[16]);
void mat4_ortho(float out[16], float left, float right, float bottom, float top, float near, float far);
void mat4_copy(float out[16], float a[16]);
void mat4_translate(float out[16], float a[16], float x, float y, float z);
void mat4_rotate_z(float out[16], float a[16], float rad);
void mat4_scale(float out[16], float a[16], float x, float y, float z);
void mat4_multiply(float out[16], float a[16], float b[16]);

#ifdef __cplusplus
}
#endif

#endif
