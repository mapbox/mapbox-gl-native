#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {
namespace gl {

static_assert(StencilMode::Never::func == GL_NEVER, "OpenGL enum mismatch");
static_assert(StencilMode::Less::func == GL_LESS, "OpenGL enum mismatch");
static_assert(StencilMode::Equal::func == GL_EQUAL, "OpenGL enum mismatch");
static_assert(StencilMode::LessEqual::func == GL_LEQUAL, "OpenGL enum mismatch");
static_assert(StencilMode::Greater::func == GL_GREATER, "OpenGL enum mismatch");
static_assert(StencilMode::NotEqual::func == GL_NOTEQUAL, "OpenGL enum mismatch");
static_assert(StencilMode::GreaterEqual::func == GL_GEQUAL, "OpenGL enum mismatch");
static_assert(StencilMode::Always::func == GL_ALWAYS, "OpenGL enum mismatch");

static_assert(underlying_type(StencilMode::Keep) == GL_KEEP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::Zero) == GL_ZERO, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::Replace) == GL_REPLACE, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::Increment) == GL_INCR, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::IncrementWrap) == GL_INCR_WRAP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::Decrement) == GL_DECR, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::DecrementWrap) == GL_DECR_WRAP, "OpenGL enum mismatch");
static_assert(underlying_type(StencilMode::Invert) == GL_INVERT, "OpenGL enum mismatch");

} // namespace gl
} // namespace mbgl
