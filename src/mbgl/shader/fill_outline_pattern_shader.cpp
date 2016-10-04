#include <mbgl/shader/fill_outline_pattern_shader.hpp>
#include <mbgl/shader/outlinepattern.vertex.hpp>
#include <mbgl/shader/outlinepattern.fragment.hpp>
#include <mbgl/shader/fill_vertex.hpp>

namespace mbgl {

FillOutlinePatternShader::FillOutlinePatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::outlinepattern::name,
             shaders::outlinepattern::vertex,
             shaders::outlinepattern::fragment,
             context, defines) {
}

} // namespace mbgl
