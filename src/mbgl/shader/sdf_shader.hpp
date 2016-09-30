#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class TextureRectVertex;

class SDFShader : public gl::Shader {
public:
    SDFShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<TextureRectVertex>&,
              const int8_t* offset);

    gl::UniformMatrix<4>              u_matrix          = {"u_matrix",          *this};
    gl::Uniform<std::array<float, 2>> u_extrude_scale   = {"u_extrude_scale",   *this};
    gl::Uniform<Color>                u_color           = {"u_color",           *this};
    gl::Uniform<float>                u_opacity         = {"u_opacity",         *this};
    gl::Uniform<std::array<float, 2>> u_texsize         = {"u_texsize",         *this};
    gl::Uniform<float>                u_buffer          = {"u_buffer",          *this};
    gl::Uniform<float>                u_gamma           = {"u_gamma",           *this};
    gl::Uniform<float>                u_zoom            = {"u_zoom",            *this};
    gl::Uniform<float>                u_pitch           = {"u_pitch",           *this};
    gl::Uniform<float>                u_bearing         = {"u_bearing",         *this};
    gl::Uniform<float>                u_aspect_ratio    = {"u_aspect_ratio",    *this};
    gl::Uniform<int32_t>              u_rotate_with_map = {"u_rotate_with_map", *this};
    gl::Uniform<int32_t>              u_pitch_with_map  = {"u_pitch_with_map",  *this};
    gl::Uniform<int32_t>              u_texture         = {"u_texture",         *this};
    gl::Uniform<int32_t>              u_fadetexture     = {"u_fadetexture",     *this};
};

} // namespace mbgl
