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

using AttributeLocation = int32_t;
using UniformLocation = int32_t;
using TextureUnit = uint8_t;

using DepthValue = double;
using StencilValue = int32_t;
using StencilMaskValue = uint32_t;

enum class DataType : uint32_t {
    Byte = 0x1400,
    UnsignedByte = 0x1401,
    Short = 0x1402,
    UnsignedShort = 0x1403,
    Integer = 0x1404,
    UnsignedInteger = 0x1405,
    Float = 0x1406
};

template <typename T> struct DataTypeOf;

template <> struct DataTypeOf<int8_t>   : std::integral_constant<DataType, DataType::Byte> {};
template <> struct DataTypeOf<uint8_t>  : std::integral_constant<DataType, DataType::UnsignedByte> {};
template <> struct DataTypeOf<int16_t>  : std::integral_constant<DataType, DataType::Short> {};
template <> struct DataTypeOf<uint16_t> : std::integral_constant<DataType, DataType::UnsignedShort> {};
template <> struct DataTypeOf<int32_t>  : std::integral_constant<DataType, DataType::Integer> {};
template <> struct DataTypeOf<uint32_t> : std::integral_constant<DataType, DataType::UnsignedInteger> {};
template <> struct DataTypeOf<float>    : std::integral_constant<DataType, DataType::Float> {};

enum class BufferType : uint32_t {
    Vertex = 0x8892,
    Element = 0x8893
};

enum class TextureMipMap : bool { No = false, Yes = true };
enum class TextureFilter : bool { Nearest = false, Linear = true };

enum class StencilTestFunction : uint32_t {
    Never = 0x0200,
    Less = 0x0201,
    Equal = 0x0202,
    LessEqual = 0x0203,
    Greater = 0x0204,
    NotEqual = 0x0205,
    GreaterEqual = 0x0206,
    Always = 0x0207,
};

enum class StencilTestOperation : uint32_t {
    Keep = 0x1E00,
    Zero = 0x0000,
    Replace = 0x1E01,
    Increment = 0x1E02,
    IncrementWrap = 0x8507,
    Decrement = 0x1E03,
    DecrementWrap = 0x8508,
    Invert = 0x150A,
};

enum class DepthTestFunction : uint32_t {
    Never = 0x0200,
    Less = 0x0201,
    Equal = 0x0202,
    LessEqual = 0x0203,
    Greater = 0x0204,
    NotEqual = 0x0205,
    GreaterEqual = 0x0206,
    Always = 0x0207,
};

enum class BlendSourceFactor : uint32_t {
    Zero = 0x0000,
    One = 0x0001,
    SrcColor = 0x0300,
    OneMinusSrcColor = 0x0301,
    DstColor = 0x0306,
    OneMinusDstColor = 0x0307,
    SrcAlpha = 0x0302,
    OneMinusSrcAlpha = 0x0303,
    DstAlpha = 0x0304,
    OneMinusDstAlpha = 0x0305,
    ConstantColor = 0x8001,
    OneMinusConstantColor = 0x8002,
    ConstantAlpha = 0x8003,
    OneMinusConstantAlpha = 0x8004,
    SrcAlphaSaturate = 0x0308,
};

enum class BlendDestinationFactor : uint32_t {
    Zero = 0x0000,
    One = 0x0001,
    SrcColor = 0x0300,
    OneMinusSrcColor = 0x0301,
    DstColor = 0x0306,
    OneMinusDstColor = 0x0307,
    SrcAlpha = 0x0302,
    OneMinusSrcAlpha = 0x0303,
    DstAlpha = 0x0304,
    OneMinusDstAlpha = 0x0305,
    ConstantColor = 0x8001,
    OneMinusConstantColor = 0x8002,
    ConstantAlpha = 0x8003,
    OneMinusConstantAlpha = 0x8004,
};

} // namespace gl
} // namespace mbgl
