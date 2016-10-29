#include <mbgl/shader/circle_shader.hpp>
#include <mbgl/shader/circle.vertex.hpp>
#include <mbgl/shader/circle.fragment.hpp>

namespace mbgl {

CircleShader::CircleShader(gl::Context& context, Defines defines)
    : Shader(shaders::circle::name,
             shaders::circle::vertex,
             shaders::circle::fragment,
             context, defines),
      attributesState(CircleAttributes::state(*this)),
      uniformsState(CircleUniforms::state(*this)) {
}

} // namespace mbgl
