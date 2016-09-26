#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {
namespace gl {

static_assert(underlying_type(DepthMode::Never) == GL_NEVER, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::Less) == GL_LESS, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::Equal) == GL_EQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::LessEqual) == GL_LEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::Greater) == GL_GREATER, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::NotEqual) == GL_NOTEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::GreaterEqual) == GL_GEQUAL, "OpenGL enum mismatch");
static_assert(underlying_type(DepthMode::Always) == GL_ALWAYS, "OpenGL enum mismatch");

} // namespace gl
} // namespace mbgl
