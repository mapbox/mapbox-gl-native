#pragma once

#include <cstdint>

namespace mbgl {
namespace gfx {

enum class PrimitiveType : uint8_t {
    Point,
    Line,
    Triangle,
};

enum class DrawModeType : uint8_t {
    Points,
    Lines,
    LineLoop,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
};

enum class AttributeDataType : uint8_t {
    Byte,
    Byte2,
    Byte3,
    Byte4,

    UByte,
    UByte2,
    UByte3,
    UByte4,

    Short,
    Short2,
    Short3,
    Short4,

    UShort,
    UShort2,
    UShort3,
    UShort4,

    Int,
    Int2,
    Int3,
    Int4,

    UInt,
    UInt2,
    UInt3,
    UInt4,

    Float,
    Float2,
    Float3,
    Float4,

    Invalid = 255,
};

enum class ColorBlendEquationType : uint8_t {
    Add,
    Subtract,
    ReverseSubtract,
};

enum class ColorBlendFactorType : uint8_t {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    DstColor,
    OneMinusDstColor,
    SrcAlphaSaturate,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
};

enum class DepthFunctionType : uint8_t {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class DepthMaskType : bool {
    ReadOnly = false,
    ReadWrite = true,
};

enum class StencilFunctionType : uint8_t {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class StencilOpType : uint8_t {
    Zero,
    Keep,
    Replace,
    Increment,
    Decrement,
    Invert,
    IncrementWrap,
    DecrementWrap,
};

enum CullFaceSideType : uint8_t {
    Front,
    Back,
    FrontAndBack,
};

enum CullFaceWindingType : uint8_t {
    Clockwise,
    CounterClockwise,
};

enum class BufferUsageType : uint8_t {
    StreamDraw,
    StaticDraw,
};

enum class TexturePixelType : uint8_t {
    RGBA,
    Alpha,
    Stencil,
    Depth,
    Luminance,
};

enum class TextureChannelDataType : uint8_t {
    UnsignedByte,
    HalfFloat,
};

enum class TextureMipMapType : bool {
    No,
    Yes
};

enum class TextureFilterType : bool {
    Nearest,
    Linear,
};

enum class TextureWrapType : bool {
    Clamp,
    Repeat,
};

enum class RenderbufferPixelType : uint8_t {
    RGBA,
    Depth,
    DepthStencil,
};

} // namespace gfx
} // namespace mbgl
