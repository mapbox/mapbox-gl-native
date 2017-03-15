#pragma once

#include <mbgl/gl/features.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>

#if MBGL_HAS_BINARY_PROGRAMS

#define GL_PROGRAM_BINARY_LENGTH                   0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS              0x87FE
#define GL_PROGRAM_BINARY_FORMATS                  0x87FF

namespace mbgl {
namespace gl {

extern ExtensionFunction<void(
    GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary)>
    GetProgramBinary;

extern ExtensionFunction<void(
    GLuint program, GLenum binaryFormat, const GLvoid* binary, GLint length)>
    ProgramBinary;

} // namespace gl
} // namespace mbgl

#endif
