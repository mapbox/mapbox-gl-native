#include <mbgl/shader/sdf_shader.hpp>
#include <mbgl/shader/sdf.vertex.hpp>
#include <mbgl/shader/sdf.fragment.hpp>
#include <mbgl/shader/texture_rect_vertex.hpp>

namespace mbgl {

SDFShader::SDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::sdf::name,
             shaders::sdf::vertex,
             shaders::sdf::fragment,
             context, defines) {
}

void SDFShader::bind(const gl::VertexBuffer<TextureRectVertex>&,
                     const int8_t* offset) {
    TextureRectVertex::bind(offset);
}

} // namespace mbgl
