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
namespace extension {

class ProgramBinary {
public:
    template <typename Fn>
    ProgramBinary(const Fn& loadExtension)
        : getProgramBinary(loadExtension({
              { "GL_OES_get_program_binary", "glGetProgramBinaryOES" },
              { "GL_ARB_get_program_binary", "glGetProgramBinary" },
          })),
          programBinary(loadExtension({
              { "GL_OES_get_program_binary", "glProgramBinaryOES" },
              { "GL_ARB_get_program_binary", "glProgramBinary" },
          })) {
    }

    const ExtensionFunction<void(
        GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary)>
        getProgramBinary;

    const ExtensionFunction<void(
        GLuint program, GLenum binaryFormat, const GLvoid* binary, GLint length)>
        programBinary;
};

} // namespace extension
} // namespace gl
} // namespace mbgl

#endif
