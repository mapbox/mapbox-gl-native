#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class IconShader : public Shader {
public:
    IconShader(gl::ObjectStore&, Defines defines = None);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix        = {"u_matrix",        *this};
    Uniform<std::array<GLfloat, 2>> u_extrude_scale = {"u_extrude_scale", *this};
    Uniform<GLfloat>                u_zoom          = {"u_zoom",          *this};
    Uniform<GLfloat>                u_opacity       = {"u_opacity",       *this};
    Uniform<std::array<GLfloat, 2>> u_texsize       = {"u_texsize",       *this};
    Uniform<GLint>                  u_rotate_with_map = {"u_rotate_with_map", *this};
    Uniform<GLint>                  u_texture       = {"u_texture",       *this};
    Uniform<GLint>                  u_fadetexture   = {"u_fadetexture",   *this};
};

} // namespace mbgl
