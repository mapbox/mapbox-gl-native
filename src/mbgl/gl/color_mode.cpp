#include <mbgl/gl/color_mode.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {
namespace gl {

static_assert(underlying_type(ColorMode::BlendEquation::Add) == GL_FUNC_ADD, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::BlendEquation::Subtract) == GL_FUNC_SUBTRACT, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::BlendEquation::ReverseSubtract) == GL_FUNC_REVERSE_SUBTRACT, "OpenGL enum mismatch");

static_assert(underlying_type(ColorMode::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::One) == GL_ONE, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::SrcColor) == GL_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusSrcColor) == GL_ONE_MINUS_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::DstColor) == GL_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusDstColor) == GL_ONE_MINUS_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::SrcAlpha) == GL_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusSrcAlpha) == GL_ONE_MINUS_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::DstAlpha) == GL_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusDstAlpha) == GL_ONE_MINUS_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::ConstantColor) == GL_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::ConstantAlpha) == GL_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::SrcAlphaSaturate) == GL_SRC_ALPHA_SATURATE, "OpenGL enum mismatch");

static_assert(underlying_type(ColorMode::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::One) == GL_ONE, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::SrcColor) == GL_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusSrcColor) == GL_ONE_MINUS_SRC_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::DstColor) == GL_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusDstColor) == GL_ONE_MINUS_DST_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::SrcAlpha) == GL_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusSrcAlpha) == GL_ONE_MINUS_SRC_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::DstAlpha) == GL_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusDstAlpha) == GL_ONE_MINUS_DST_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::ConstantColor) == GL_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::ConstantAlpha) == GL_CONSTANT_ALPHA, "OpenGL enum mismatch");
static_assert(underlying_type(ColorMode::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA, "OpenGL enum mismatch");

} // namespace gl
} // namespace mbgl
