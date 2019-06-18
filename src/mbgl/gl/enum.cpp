#include <mbgl/gl/enum.hpp>
#include <mbgl/gfx/types.hpp>
#include <mbgl/gl/defines.hpp>

namespace mbgl {
namespace gl {

template <>
platform::GLenum Enum<gfx::DrawModeType>::to(const gfx::DrawModeType value) {
    switch (value) {
        case gfx::DrawModeType::Points: return GL_POINTS;
        case gfx::DrawModeType::Lines: return GL_LINES;
        case gfx::DrawModeType::LineLoop: return GL_LINE_LOOP;
        case gfx::DrawModeType::LineStrip: return GL_LINE_STRIP;
        case gfx::DrawModeType::Triangles: return GL_TRIANGLES;
        case gfx::DrawModeType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case gfx::DrawModeType::TriangleFan: return GL_TRIANGLE_FAN;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::ColorBlendEquationType Enum<gfx::ColorBlendEquationType>::from(const platform::GLint value) {
    switch (value) {
        case GL_FUNC_ADD: return gfx::ColorBlendEquationType::Add;
        case GL_FUNC_SUBTRACT: return gfx::ColorBlendEquationType::Subtract;
        case GL_FUNC_REVERSE_SUBTRACT: return gfx::ColorBlendEquationType::ReverseSubtract;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::ColorBlendEquationType>::to(const gfx::ColorBlendEquationType value) {
    switch (value) {
        case gfx::ColorBlendEquationType::Add: return GL_FUNC_ADD;
        case gfx::ColorBlendEquationType::Subtract: return GL_FUNC_SUBTRACT;
        case gfx::ColorBlendEquationType::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::ColorBlendFactorType Enum<gfx::ColorBlendFactorType>::from(const platform::GLint value) {
    switch (value) {
        case GL_ZERO: return gfx::ColorBlendFactorType::Zero;
        case GL_ONE: return gfx::ColorBlendFactorType::One;
        case GL_SRC_COLOR: return gfx::ColorBlendFactorType::SrcColor;
        case GL_ONE_MINUS_SRC_COLOR: return gfx::ColorBlendFactorType::OneMinusSrcColor;
        case GL_DST_COLOR: return gfx::ColorBlendFactorType::DstColor;
        case GL_ONE_MINUS_DST_COLOR: return gfx::ColorBlendFactorType::OneMinusDstColor;
        case GL_SRC_ALPHA: return gfx::ColorBlendFactorType::SrcAlpha;
        case GL_ONE_MINUS_SRC_ALPHA: return gfx::ColorBlendFactorType::OneMinusSrcAlpha;
        case GL_DST_ALPHA: return gfx::ColorBlendFactorType::DstAlpha;
        case GL_ONE_MINUS_DST_ALPHA: return gfx::ColorBlendFactorType::OneMinusDstAlpha;
        case GL_CONSTANT_COLOR: return gfx::ColorBlendFactorType::ConstantColor;
        case GL_ONE_MINUS_CONSTANT_COLOR: return gfx::ColorBlendFactorType::OneMinusConstantColor;
        case GL_CONSTANT_ALPHA: return gfx::ColorBlendFactorType::ConstantAlpha;
        case GL_ONE_MINUS_CONSTANT_ALPHA: return gfx::ColorBlendFactorType::OneMinusConstantAlpha;
        case GL_SRC_ALPHA_SATURATE: return gfx::ColorBlendFactorType::SrcAlphaSaturate;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::ColorBlendFactorType>::to(const gfx::ColorBlendFactorType value) {
    switch (value) {
        case gfx::ColorBlendFactorType::Zero: return GL_ZERO;
        case gfx::ColorBlendFactorType::One: return GL_ONE;
        case gfx::ColorBlendFactorType::SrcColor: return GL_SRC_COLOR;
        case gfx::ColorBlendFactorType::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
        case gfx::ColorBlendFactorType::DstColor: return GL_DST_COLOR;
        case gfx::ColorBlendFactorType::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
        case gfx::ColorBlendFactorType::SrcAlpha: return GL_SRC_ALPHA;
        case gfx::ColorBlendFactorType::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case gfx::ColorBlendFactorType::DstAlpha: return GL_DST_ALPHA;
        case gfx::ColorBlendFactorType::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
        case gfx::ColorBlendFactorType::ConstantColor: return GL_CONSTANT_COLOR;
        case gfx::ColorBlendFactorType::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
        case gfx::ColorBlendFactorType::ConstantAlpha: return GL_CONSTANT_ALPHA;
        case gfx::ColorBlendFactorType::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case gfx::ColorBlendFactorType::SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::DepthFunctionType Enum<gfx::DepthFunctionType>::from(const platform::GLint value) {
    switch (value) {
        case GL_NEVER: return gfx::DepthFunctionType::Never;
        case GL_LESS: return gfx::DepthFunctionType::Less;
        case GL_EQUAL: return gfx::DepthFunctionType::Equal;
        case GL_LEQUAL: return gfx::DepthFunctionType::LessEqual;
        case GL_GREATER: return gfx::DepthFunctionType::Greater;
        case GL_NOTEQUAL: return gfx::DepthFunctionType::NotEqual;
        case GL_GEQUAL: return gfx::DepthFunctionType::GreaterEqual;
        case GL_ALWAYS: return gfx::DepthFunctionType::Always;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::DepthFunctionType>::to(const gfx::DepthFunctionType value) {
    switch (value) {
        case gfx::DepthFunctionType::Never: return GL_NEVER;
        case gfx::DepthFunctionType::Less: return GL_LESS;
        case gfx::DepthFunctionType::Equal: return GL_EQUAL;
        case gfx::DepthFunctionType::LessEqual: return GL_LEQUAL;
        case gfx::DepthFunctionType::Greater: return GL_GREATER;
        case gfx::DepthFunctionType::NotEqual: return GL_NOTEQUAL;
        case gfx::DepthFunctionType::GreaterEqual: return GL_GEQUAL;
        case gfx::DepthFunctionType::Always: return GL_ALWAYS;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::DepthMaskType Enum<gfx::DepthMaskType>::from(const platform::GLboolean value) {
    return value ? gfx::DepthMaskType::ReadWrite : gfx::DepthMaskType::ReadOnly;
}

template <>
platform::GLboolean Enum<gfx::DepthMaskType>::to(const gfx::DepthMaskType value) {
    return value == gfx::DepthMaskType::ReadWrite ? GL_TRUE : GL_FALSE;
}

template <>
gfx::StencilFunctionType Enum<gfx::StencilFunctionType>::from(const platform::GLint value) {
    switch (value) {
        case GL_NEVER: return gfx::StencilFunctionType::Never;
        case GL_LESS: return gfx::StencilFunctionType::Less;
        case GL_EQUAL: return gfx::StencilFunctionType::Equal;
        case GL_LEQUAL: return gfx::StencilFunctionType::LessEqual;
        case GL_GREATER: return gfx::StencilFunctionType::Greater;
        case GL_NOTEQUAL: return gfx::StencilFunctionType::NotEqual;
        case GL_GEQUAL: return gfx::StencilFunctionType::GreaterEqual;
        case GL_ALWAYS: return gfx::StencilFunctionType::Always;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::StencilFunctionType>::to(const gfx::StencilFunctionType value) {
    switch (value) {
        case gfx::StencilFunctionType::Never: return GL_NEVER;
        case gfx::StencilFunctionType::Less: return GL_LESS;
        case gfx::StencilFunctionType::Equal: return GL_EQUAL;
        case gfx::StencilFunctionType::LessEqual: return GL_LEQUAL;
        case gfx::StencilFunctionType::Greater: return GL_GREATER;
        case gfx::StencilFunctionType::NotEqual: return GL_NOTEQUAL;
        case gfx::StencilFunctionType::GreaterEqual: return GL_GEQUAL;
        case gfx::StencilFunctionType::Always: return GL_ALWAYS;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::StencilOpType Enum<gfx::StencilOpType>::from(const platform::GLint value) {
    switch (value) {
        case GL_KEEP: return gfx::StencilOpType::Keep;
        case GL_ZERO: return gfx::StencilOpType::Zero;
        case GL_REPLACE: return gfx::StencilOpType::Replace;
        case GL_INCR: return gfx::StencilOpType::Increment;
        case GL_INCR_WRAP: return gfx::StencilOpType::IncrementWrap;
        case GL_DECR: return gfx::StencilOpType::Decrement;
        case GL_DECR_WRAP: return gfx::StencilOpType::DecrementWrap;
        case GL_INVERT: return gfx::StencilOpType::Invert;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::StencilOpType>::to(const gfx::StencilOpType value) {
    switch (value) {
        case gfx::StencilOpType::Keep: return GL_KEEP;
        case gfx::StencilOpType::Zero: return GL_ZERO;
        case gfx::StencilOpType::Replace: return GL_REPLACE;
        case gfx::StencilOpType::Increment: return GL_INCR;
        case gfx::StencilOpType::IncrementWrap: return GL_INCR_WRAP;
        case gfx::StencilOpType::Decrement: return GL_DECR;
        case gfx::StencilOpType::DecrementWrap: return GL_DECR_WRAP;
        case gfx::StencilOpType::Invert: return GL_INVERT;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::CullFaceSideType Enum<gfx::CullFaceSideType>::from(const platform::GLint value) {
    switch (value) {
        case GL_FRONT: return gfx::CullFaceSideType::Front;
        case GL_BACK: return gfx::CullFaceSideType::Back;
        case GL_FRONT_AND_BACK: return gfx::CullFaceSideType::FrontAndBack;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::CullFaceSideType>::to(const gfx::CullFaceSideType value) {
    switch (value) {
        case gfx::CullFaceSideType::Front: return GL_FRONT;
        case gfx::CullFaceSideType::Back: return GL_BACK;
        case gfx::CullFaceSideType::FrontAndBack: return GL_FRONT_AND_BACK;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::CullFaceWindingType Enum<gfx::CullFaceWindingType>::from(const platform::GLint value) {
    switch (value) {
        case GL_CW: return gfx::CullFaceWindingType::Clockwise;
        case GL_CCW: return gfx::CullFaceWindingType::CounterClockwise;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::CullFaceWindingType>::to(const gfx::CullFaceWindingType value) {
    switch (value) {
        case gfx::CullFaceWindingType::Clockwise: return GL_CW;
        case gfx::CullFaceWindingType::CounterClockwise: return GL_CCW;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::BufferUsageType Enum<gfx::BufferUsageType>::from(const platform::GLint value) {
    switch (value) {
        case GL_STREAM_DRAW: return gfx::BufferUsageType::StreamDraw;
        case GL_STATIC_DRAW: return gfx::BufferUsageType::StaticDraw;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::BufferUsageType>::to(const gfx::BufferUsageType value) {
    switch (value) {
        case gfx::BufferUsageType::StreamDraw: return GL_STREAM_DRAW;
        case gfx::BufferUsageType::StaticDraw: return GL_STATIC_DRAW;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::TexturePixelType Enum<gfx::TexturePixelType>::from(const platform::GLint value) {
    switch (value) {
        case GL_RGBA: return gfx::TexturePixelType::RGBA;
        case GL_ALPHA: return gfx::TexturePixelType::Alpha;
        case GL_STENCIL_INDEX: return gfx::TexturePixelType::Stencil;
        case GL_DEPTH_COMPONENT: return gfx::TexturePixelType::Depth;
        case GL_LUMINANCE: return gfx::TexturePixelType::Luminance;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::TexturePixelType>::to(const gfx::TexturePixelType value) {
    switch (value) {
        case gfx::TexturePixelType::RGBA: return GL_RGBA;
        case gfx::TexturePixelType::Alpha: return GL_ALPHA;
        case gfx::TexturePixelType::Stencil: return GL_STENCIL_INDEX;
        case gfx::TexturePixelType::Depth: return GL_DEPTH_COMPONENT;
        case gfx::TexturePixelType::Luminance: return GL_LUMINANCE;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::TextureChannelDataType Enum<gfx::TextureChannelDataType>::from(const platform::GLint value) {
    switch (value) {
        case GL_UNSIGNED_BYTE: return gfx::TextureChannelDataType::UnsignedByte;
        case GL_HALF_FLOAT: return gfx::TextureChannelDataType::HalfFloat;
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::TextureChannelDataType>::to(const gfx::TextureChannelDataType value) {
    switch (value) {
        case gfx::TextureChannelDataType::UnsignedByte: return GL_UNSIGNED_BYTE;
        case gfx::TextureChannelDataType::HalfFloat: return GL_HALF_FLOAT;
    }
    return GL_INVALID_ENUM;
}

template <>
gfx::RenderbufferPixelType Enum<gfx::RenderbufferPixelType>::from(const platform::GLint value) {
    switch (value) {
#if not MBGL_USE_GLES2
        case GL_RGBA8: return gfx::RenderbufferPixelType::RGBA;
        case GL_DEPTH_COMPONENT: return gfx::RenderbufferPixelType::Depth;
        case GL_DEPTH24_STENCIL8: return gfx::RenderbufferPixelType::DepthStencil;
#else
        case GL_RGBA8_OES: return gfx::RenderbufferPixelType::RGBA;
        case GL_DEPTH_COMPONENT16: return gfx::RenderbufferPixelType::Depth;
        case GL_DEPTH24_STENCIL8_OES: return gfx::RenderbufferPixelType::DepthStencil;
#endif
    }
    return {};
}

template <>
platform::GLenum Enum<gfx::RenderbufferPixelType>::to(const gfx::RenderbufferPixelType value) {
    switch (value) {
#if not MBGL_USE_GLES2
        case gfx::RenderbufferPixelType::RGBA: return GL_RGBA8;
        case gfx::RenderbufferPixelType::Depth: return GL_DEPTH_COMPONENT;
        case gfx::RenderbufferPixelType::DepthStencil: return GL_DEPTH24_STENCIL8;
#else
        case gfx::RenderbufferPixelType::RGBA: return GL_RGBA8_OES;
        case gfx::RenderbufferPixelType::Depth: return GL_DEPTH_COMPONENT16;
        case gfx::RenderbufferPixelType::DepthStencil: return GL_DEPTH24_STENCIL8_OES;
#endif
    }
    return GL_INVALID_ENUM;
}

} // namespace gl
} // namespace mbgl
