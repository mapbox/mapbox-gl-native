#include <mbgl/shader/line_pattern_shader.hpp>
#include <mbgl/shader/line_pattern.vertex.hpp>
#include <mbgl/shader/line_pattern.fragment.hpp>
#include <mbgl/shader/line_vertex.hpp>

namespace mbgl {

LinePatternShader::LinePatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::line_pattern::name,
             shaders::line_pattern::vertex,
             shaders::line_pattern::fragment,
             context, defines) {
}

} // namespace mbgl
