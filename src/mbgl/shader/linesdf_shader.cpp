#include <mbgl/shader/linesdf_shader.hpp>
#include <mbgl/shader/linesdfpattern.vertex.hpp>
#include <mbgl/shader/linesdfpattern.fragment.hpp>
#include <mbgl/shader/line_vertex.hpp>

namespace mbgl {

LineSDFShader::LineSDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::linesdfpattern::name,
             shaders::linesdfpattern::vertex,
             shaders::linesdfpattern::fragment,
             context, defines) {
}

} // namespace mbgl
