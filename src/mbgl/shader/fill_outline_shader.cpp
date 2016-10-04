#include <mbgl/shader/fill_outline_shader.hpp>
#include <mbgl/shader/outline.vertex.hpp>
#include <mbgl/shader/outline.fragment.hpp>
#include <mbgl/shader/fill_vertex.hpp>

namespace mbgl {

FillOutlineShader::FillOutlineShader(gl::Context& context, Defines defines)
    : Shader(shaders::outline::name,
             shaders::outline::vertex,
             shaders::outline::fragment,
             context, defines) {
}

} // namespace mbgl
