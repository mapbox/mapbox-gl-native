#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class OutlineShader : public Shader {
public:
    OutlineShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix         = {"u_matrix",        *this};
    Uniform<Color>                  u_outline_color  = {"u_outline_color", *this};
    Uniform<GLfloat>                u_opacity        = {"u_opacity",       *this};
    Uniform<std::array<GLfloat, 2>> u_world          = {"u_world",         *this};
};

} // namespace mbgl
