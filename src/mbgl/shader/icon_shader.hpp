#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class TextureRectVertex;

class IconShader : public gl::Shader {
public:
    IconShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<TextureRectVertex>&,
              const int8_t* offset);

    gl::UniformMatrix<4>              u_matrix          = {"u_matrix",          *this};
    gl::Uniform<std::array<float, 2>> u_extrude_scale   = {"u_extrude_scale",   *this};
    gl::Uniform<float>                u_zoom            = {"u_zoom",            *this};
    gl::Uniform<float>                u_opacity         = {"u_opacity",         *this};
    gl::Uniform<std::array<float, 2>> u_texsize         = {"u_texsize",         *this};
    gl::Uniform<int32_t>              u_rotate_with_map = {"u_rotate_with_map", *this};
    gl::Uniform<int32_t>              u_texture         = {"u_texture",         *this};
    gl::Uniform<int32_t>              u_fadetexture     = {"u_fadetexture",     *this};
};

} // namespace mbgl
