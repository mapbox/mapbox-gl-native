#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class OutlineShader : public Shader {
public:
    OutlineShader(gl::GLObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix = {"u_matrix", *this};
    Uniform<std::array<GLfloat, 4>> u_color  = {"u_color",  *this};
    Uniform<GLfloat>                u_opacity = {"u_opacity", *this};
    Uniform<std::array<GLfloat, 2>> u_world  = {"u_world",  *this};
};

} // namespace mbgl
