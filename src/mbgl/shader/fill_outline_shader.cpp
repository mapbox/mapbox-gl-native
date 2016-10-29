#include <mbgl/shader/fill_outline_shader.hpp>
#include <mbgl/shader/fill_outline.vertex.hpp>
#include <mbgl/shader/fill_outline.fragment.hpp>

namespace mbgl {

FillOutlineShader::FillOutlineShader(gl::Context& context, Defines defines)
    : Shader(shaders::fill_outline::name,
             shaders::fill_outline::vertex,
             shaders::fill_outline::fragment,
             context, defines),
      attributesState(FillAttributes::state(*this)),
      uniformsState(FillColorUniforms::state(*this)) {
}

} // namespace mbgl
