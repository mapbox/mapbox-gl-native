#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class TextureRectVertex;

class IconShader : public Shader {
public:
    IconShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<TextureRectVertex>&,
              const int8_t* offset);

    UniformMatrix<4>              u_matrix          = {"u_matrix",          *this};
    Uniform<std::array<float, 2>> u_extrude_scale   = {"u_extrude_scale",   *this};
    Uniform<float>                u_zoom            = {"u_zoom",            *this};
    Uniform<float>                u_opacity         = {"u_opacity",         *this};
    Uniform<std::array<float, 2>> u_texsize         = {"u_texsize",         *this};
    Uniform<int32_t>              u_rotate_with_map = {"u_rotate_with_map", *this};
    Uniform<int32_t>              u_texture         = {"u_texture",         *this};
    Uniform<int32_t>              u_fadetexture     = {"u_fadetexture",     *this};
};

} // namespace mbgl
