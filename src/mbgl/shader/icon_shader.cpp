#include <mbgl/shader/icon_shader.hpp>
#include <mbgl/shader/icon.vertex.hpp>
#include <mbgl/shader/icon.fragment.hpp>
#include <mbgl/shader/texture_rect_vertex.hpp>

namespace mbgl {

IconShader::IconShader(gl::Context& context, Defines defines)
    : Shader(shaders::icon::name,
             shaders::icon::vertex,
             shaders::icon::fragment,
             context, defines) {
}

} // namespace mbgl
