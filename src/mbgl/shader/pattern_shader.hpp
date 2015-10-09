#ifndef MBGL_SHADER_SHADER_PATTERN
#define MBGL_SHADER_SHADER_PATTERN

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class PatternShader : public Shader {
public:
    PatternShader();

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix          = {"u_matrix",          *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_tl_a    = {"u_pattern_tl_a",    *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_br_a    = {"u_pattern_br_a",    *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_tl_b    = {"u_pattern_tl_b",    *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_br_b    = {"u_pattern_br_b",    *this};
    Uniform<GLfloat>                u_opacity         = {"u_opacity",         *this};
    Uniform<GLfloat>                u_mix             = {"u_mix",             *this};
    Uniform<GLint>                  u_image           = {"u_image",           *this};
    UniformMatrix<3>                u_patternmatrix_a = {"u_patternmatrix_a", *this};
    UniformMatrix<3>                u_patternmatrix_b = {"u_patternmatrix_b", *this};
};

}

#endif
