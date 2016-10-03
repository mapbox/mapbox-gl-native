#include <mbgl/shader/pattern_shader.hpp>
#include <mbgl/shader/pattern.vertex.hpp>
#include <mbgl/shader/pattern.fragment.hpp>
#include <mbgl/shader/plain_vertex.hpp>

namespace mbgl {

PatternShader::PatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::pattern::name,
             shaders::pattern::vertex,
             shaders::pattern::fragment,
             context, defines) {
}

} // namespace mbgl
