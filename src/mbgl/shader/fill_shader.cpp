#include <mbgl/shader/fill_shader.hpp>
#include <mbgl/shader/fill.vertex.hpp>
#include <mbgl/shader/fill.fragment.hpp>

namespace mbgl {

FillShader::FillShader(gl::Context& context, Defines defines)
    : Shader(shaders::fill::name,
             shaders::fill::vertex,
             shaders::fill::fragment,
             context, defines),
      attributesState(FillAttributes::state(*this)),
      uniformsState(FillColorUniforms::state(*this)) {
}

} // namespace mbgl
