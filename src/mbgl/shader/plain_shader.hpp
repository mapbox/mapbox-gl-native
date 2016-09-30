#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class PlainShader : public Shader {
public:
    PlainShader(gl::Context&, Defines defines = None);

    void bind(int8_t* offset) final;

    UniformMatrix<4> u_matrix   = {"u_matrix",  *this};
    Uniform<Color>   u_color    = {"u_color",   *this};
    Uniform<float>   u_opacity  = {"u_opacity", *this};
};

} // namespace mbgl
