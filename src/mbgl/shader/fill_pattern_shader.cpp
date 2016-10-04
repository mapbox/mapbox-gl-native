#include <mbgl/shader/fill_pattern_shader.hpp>
#include <mbgl/shader/pattern.vertex.hpp>
#include <mbgl/shader/pattern.fragment.hpp>
#include <mbgl/shader/fill_vertex.hpp>

namespace mbgl {

FillPatternShader::FillPatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::pattern::name,
             shaders::pattern::vertex,
             shaders::pattern::fragment,
             context, defines) {
}

} // namespace mbgl
