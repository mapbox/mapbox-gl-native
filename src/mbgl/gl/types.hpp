#pragma once

#include <cstdint>
#include <type_traits>

namespace mbgl {
namespace gl {

// Mapping based on https://www.opengl.org/wiki/OpenGL_Type
using ProgramID = uint32_t;
using ShaderID = uint32_t;
using BufferID = uint32_t;
using TextureID = uint32_t;
using VertexArrayID = uint32_t;
using FramebufferID = uint32_t;
using RenderbufferID = uint32_t;

// OpenGL does not formally define a type for attribute locations, but most APIs use
// GLuint. The exception is glGetAttribLocation, which returns GLint so that -1 can
// be used as an error indicator.
using AttributeLocation = uint32_t;

// OpenGL does not formally define a type for uniform locations, but all APIs use GLint.
// The value -1 is special, typically used as a placeholder for an unused uniform and
// "silently ignored".
using UniformLocation = int32_t;

enum class ShaderType : uint32_t {
    Vertex = 0x8B31,
    Fragment = 0x8B30
};

struct PixelStorageType {
    int32_t alignment;
};

constexpr bool operator!=(const PixelStorageType& a, const PixelStorageType& b) {
    return a.alignment != b.alignment;
}

enum class UniformDataType : uint32_t {
    Float = 0x1406,
    FloatVec2 = 0x8B50,
    FloatVec3 = 0x8B51,
    FloatVec4 = 0x8B52,
    Int = 0x1404,
    IntVec2 = 0x8B53,
    IntVec3 = 0x8B54,
    IntVec4 = 0x8B55,
    Bool = 0x8B56,
    BoolVec2 = 0x8B57,
    BoolVec3 = 0x8B58,
    BoolVec4 = 0x8B59,
    FloatMat2 = 0x8B5A,
    FloatMat3 = 0x8B5B,
    FloatMat4 = 0x8B5C,
    Sampler2D = 0x8B5E,
    SamplerCube = 0x8B60,
};

} // namespace gl
} // namespace mbgl
