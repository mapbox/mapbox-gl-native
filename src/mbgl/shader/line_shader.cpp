#include <mbgl/shader/line_shader.hpp>
#include <mbgl/shader/line.vertex.hpp>
#include <mbgl/shader/line.fragment.hpp>
#include <mbgl/shader/line_vertex.hpp>

namespace mbgl {

LineShader::LineShader(gl::Context& context, Defines defines)
    : Shader(shaders::line::name,
             shaders::line::vertex,
             shaders::line::fragment,
             context, defines) {
}

} // namespace mbgl
