#ifndef MBGL_SHADER_CIRCLE_SHADER
#define MBGL_SHADER_CIRCLE_SHADER

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class CircleShader : public Shader {
public:
    CircleShader();

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                 u_matrix   = {"u_matrix",   *this};
    UniformMatrix<4>                 u_exmatrix = {"u_exmatrix", *this};
    Uniform<std::array<GLfloat, 4>>  u_color    = {"u_color",    *this};
    Uniform<GLfloat>                 u_size     = {"u_size",     *this};
    Uniform<GLfloat>                 u_blur     = {"u_blur",     *this};
};

}

#endif // MBGL_SHADER_CIRCLE_SHADER
