#include <mbgl/shader/outlinepattern_shader.hpp>
#include <mbgl/shader/outlinepattern.vertex.hpp>
#include <mbgl/shader/outlinepattern.fragment.hpp>
#include <mbgl/shader/plain_vertex.hpp>

namespace mbgl {

OutlinePatternShader::OutlinePatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::outlinepattern::name,
             shaders::outlinepattern::vertex,
             shaders::outlinepattern::fragment,
             context, defines) {
}

} // namespace mbgl
