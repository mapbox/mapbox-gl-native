#include <mbgl/gl/enum.hpp>
#include <mbgl/gfx/types.hpp>
#include <mbgl/gl/defines.hpp>

namespace mbgl {
namespace gl {

template <>
platform::GLenum Enum<gfx::PrimitiveType>::to(const gfx::PrimitiveType value) {
    switch (value) {
        case gfx::PrimitiveType::Points: return GL_POINTS;
        case gfx::PrimitiveType::Lines: return GL_LINES;
        case gfx::PrimitiveType::LineLoop: return GL_LINE_LOOP;
        case gfx::PrimitiveType::LineStrip: return GL_LINE_STRIP;
        case gfx::PrimitiveType::Triangles: return GL_TRIANGLES;
        case gfx::PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case gfx::PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
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

} // namespace gl
} // namespace mbgl
