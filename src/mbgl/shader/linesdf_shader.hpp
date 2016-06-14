#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class LineSDFShader : public Shader {
public:
    LineSDFShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                 u_matrix    = {"u_matrix",    *this};
    Uniform<Color>                   u_color     = {"u_color",     *this};
    Uniform<GLfloat>                 u_opacity   = {"u_opacity",   *this};
    Uniform<GLfloat>                 u_linewidth = {"u_linewidth", *this};
    Uniform<GLfloat>                 u_gapwidth  = {"u_gapwidth",  *this};
    Uniform<GLfloat>                 u_antialiasing = {"u_antialiasing",  *this};
    Uniform<GLfloat>                 u_ratio     = {"u_ratio",     *this};
    Uniform<GLfloat>                 u_blur      = {"u_blur",      *this};
    Uniform<std::array<GLfloat, 2>>  u_patternscale_a = { "u_patternscale_a", *this};
    Uniform<GLfloat>                 u_tex_y_a   = {"u_tex_y_a",   *this};
    Uniform<std::array<GLfloat, 2>>  u_patternscale_b = { "u_patternscale_b", *this};
    Uniform<GLfloat>                 u_tex_y_b   = {"u_tex_y_b",   *this};
    Uniform<GLint>                   u_image     = {"u_image",     *this};
    Uniform<GLfloat>                 u_sdfgamma  = {"u_sdfgamma",  *this};
    Uniform<GLfloat>                 u_mix       = {"u_mix",       *this};
    Uniform<GLfloat>                 u_extra     = {"u_extra",     *this};
    Uniform<GLfloat>                 u_offset    = {"u_offset",    *this};
    UniformMatrix<2>                 u_antialiasingmatrix = {"u_antialiasingmatrix", *this};

private:
    GLint a_data = -1;
};


} // namespace mbgl
