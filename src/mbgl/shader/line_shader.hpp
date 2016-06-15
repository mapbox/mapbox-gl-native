#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class LineShader : public Shader {
public:
    LineShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                 u_matrix    = {"u_matrix",    *this};
    Uniform<Color>                   u_color     = {"u_color",     *this};
    Uniform<GLfloat>                 u_opacity   = {"u_opacity",   *this};
    Uniform<GLfloat>                 u_ratio     = {"u_ratio",     *this};
    Uniform<GLfloat>                 u_linewidth = {"u_linewidth", *this};
    Uniform<GLfloat>                 u_gapwidth  = {"u_gapwidth",  *this};
    Uniform<GLfloat>                 u_antialiasing = {"u_antialiasing",  *this};
    Uniform<GLfloat>                 u_blur      = {"u_blur",      *this};
    Uniform<GLfloat>                 u_extra     = {"u_extra",     *this};
    Uniform<GLfloat>                 u_offset    = {"u_offset",    *this};
    UniformMatrix<2>                 u_antialiasingmatrix  = {"u_antialiasingmatrix",  *this};

private:
    GLint a_data = -1;
};


} // namespace mbgl
