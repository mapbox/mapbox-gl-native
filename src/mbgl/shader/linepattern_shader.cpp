#include <mbgl/shader/linepattern_shader.hpp>
#include <mbgl/shader/linepattern.vertex.hpp>
#include <mbgl/shader/linepattern.fragment.hpp>
#include <mbgl/shader/line_vertex.hpp>

namespace mbgl {

LinepatternShader::LinepatternShader(gl::Context& context, Defines defines)
    : Shader(shaders::linepattern::name,
             shaders::linepattern::vertex,
             shaders::linepattern::fragment,
             context, defines) {
}

void LinepatternShader::bind(const gl::VertexBuffer<LineVertex>&,
                             const int8_t* offset) {
    LineVertex::bind(offset);
}

} // namespace mbgl
