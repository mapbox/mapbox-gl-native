#include <mbgl/gl/cull_face_mode.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/traits.hpp>

namespace mbgl {
namespace gl {

static_assert(underlying_type(CullFaceMode::Front) == GL_FRONT, "OpenGL enum mismatch");
static_assert(underlying_type(CullFaceMode::Back) == GL_BACK, "OpenGL enum mismatch");
static_assert(underlying_type(CullFaceMode::FrontAndBack) == GL_FRONT_AND_BACK, "OpenGL enum mismatch");

static_assert(underlying_type(CullFaceMode::Clockwise) == GL_CW, "OpenGL enum mismatch");
static_assert(underlying_type(CullFaceMode::CounterClockwise) == GL_CCW, "OpenGL enum mismatch");

} // namespace gl
} // namespace mbgl
