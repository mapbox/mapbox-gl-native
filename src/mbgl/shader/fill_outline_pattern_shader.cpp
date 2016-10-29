#include <mbgl/shader/fill_outline_pattern_shader.hpp>
#include <mbgl/shader/fill_outline_pattern.vertex.hpp>
#include <mbgl/shader/fill_outline_pattern.fragment.hpp>

namespace mbgl {

FillOutlinePatternShader::FillOutlinePatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::fill_outline_pattern::name,
             shaders::fill_outline_pattern::vertex,
             shaders::fill_outline_pattern::fragment,
             context, defines),
      attributesState(FillAttributes::state(*this)),
      uniformsState(FillPatternUniforms::state(*this)) {
}

} // namespace mbgl
