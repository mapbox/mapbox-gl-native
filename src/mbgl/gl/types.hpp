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

using TextureUnit = uint8_t;

enum class ShaderType : uint32_t {
    Vertex = 0x8B31,
    Fragment = 0x8B30
};

enum class DataType : uint16_t {
    Byte = 0x1400,
    UnsignedByte = 0x1401,
    Short = 0x1402,
    UnsignedShort = 0x1403,
    Integer = 0x1404,
    UnsignedInteger = 0x1405,
    Float = 0x1406
};

enum class RenderbufferType : uint32_t {
    RGBA = 0x8058,
    DepthStencil = 0x88F0,
#if not MBGL_USE_GLES2
    DepthComponent = 0x1902,     // GL_DEPTH_COMPONENT
#else
    DepthComponent = 0x81A5,     // GL_DEPTH_COMPONENT16
#endif // MBGL_USE_GLES2
};

enum class TextureMipMap : bool { No = false, Yes = true };
enum class TextureFilter : bool { Nearest = false, Linear = true };
enum class TextureWrap : bool { Clamp, Repeat };
enum class TextureFormat : uint32_t {
    RGBA = 0x1908,
    Alpha = 0x1906,
#if not MBGL_USE_GLES2
    Stencil = 0x1901,
    Depth = 0x1902,
#endif // MBGL_USE_GLES2
};

enum class TextureType : uint32_t {
    UnsignedByte = 0x1401,
#if MBGL_USE_GLES2
    HalfFloat = 0x8D61,
#else
    HalfFloat = 0x140B,
#endif // MBGL_USE_GLES2
};

enum class PrimitiveType {
    Points = 0x0000,
    Lines = 0x0001,
    LineLoop = 0x0002,
    LineStrip = 0x0003,
    Triangles = 0x0004,
    TriangleStrip = 0x0005,
    TriangleFan = 0x0006
};

struct PixelStorageType {
    int32_t alignment;
};

constexpr bool operator!=(const PixelStorageType& a, const PixelStorageType& b) {
    return a.alignment != b.alignment;
}

using BinaryProgramFormat = uint32_t;

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

enum class BufferUsage : uint32_t {
    StreamDraw = 0x88E0,
    StaticDraw = 0x88E4,
    DynamicDraw = 0x88E8,
};

} // namespace gl
} // namespace mbgl
