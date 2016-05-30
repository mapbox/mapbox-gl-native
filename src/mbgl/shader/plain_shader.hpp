#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class PlainShader : public Shader {
public:
    PlainShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix   = {"u_matrix", *this};
    Uniform<std::array<GLfloat, 4>> u_color    = {"u_color",  *this};
    Uniform<GLfloat>                u_opacity  = {"u_opacity", *this};
};

} // namespace mbgl
