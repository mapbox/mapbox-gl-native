#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class CircleShader : public Shader {
public:
    CircleShader(gl::ObjectStore&, Defines defines = None);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                 u_matrix           = {"u_matrix",           *this};
    Uniform<std::array<GLfloat, 2>>  u_extrude_scale    = {"u_extrude_scale",    *this};
    Uniform<GLfloat>                 u_devicepixelratio = {"u_devicepixelratio", *this};
    Uniform<Color>                   u_color            = {"u_color",            *this};
    Uniform<GLfloat>                 u_radius           = {"u_radius",           *this};
    Uniform<GLfloat>                 u_blur             = {"u_blur",             *this};
    Uniform<GLfloat>                 u_opacity          = {"u_opacity",          *this};
    Uniform<GLint>                   u_scale_with_map   = {"u_scale_with_map",   *this};
};

} // namespace mbgl
