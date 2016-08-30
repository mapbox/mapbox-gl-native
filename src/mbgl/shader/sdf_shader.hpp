#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class SDFShader : public Shader {
public:
    SDFShader(gl::ObjectStore&, Defines defines = None);

    UniformMatrix<4>                u_matrix        = {"u_matrix",        *this};
    Uniform<std::array<GLfloat, 2>> u_extrude_scale = {"u_extrude_scale", *this};
    Uniform<Color>                  u_color         = {"u_color",         *this};
    Uniform<GLfloat>                u_opacity       = {"u_opacity",       *this};
    Uniform<std::array<GLfloat, 2>> u_texsize       = {"u_texsize",       *this};
    Uniform<GLfloat>                u_buffer        = {"u_buffer",        *this};
    Uniform<GLfloat>                u_gamma         = {"u_gamma",         *this};
    Uniform<GLfloat>                u_zoom          = {"u_zoom",          *this};
    Uniform<GLfloat>                u_pitch         = {"u_pitch",         *this};
    Uniform<GLfloat>                u_bearing       = {"u_bearing",       *this};
    Uniform<GLfloat>                u_aspect_ratio  = {"u_aspect_ratio",  *this};
    Uniform<GLint>                  u_rotate_with_map = {"u_rotate_with_map",*this};
    Uniform<GLint>                  u_pitch_with_map = {"u_pitch_with_map",*this};
    Uniform<GLint>                  u_texture       = {"u_texture",       *this};
    Uniform<GLint>                  u_fadetexture   = {"u_fadetexture",   *this};

    void bind(GLbyte *offset) final;
};

} // namespace mbgl
