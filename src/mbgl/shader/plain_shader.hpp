#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class PlainVertex;

class PlainShader : public Shader {
public:
    PlainShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<PlainVertex>&,
              const int8_t* offset);

    UniformMatrix<4> u_matrix   = {"u_matrix",  *this};
    Uniform<Color>   u_color    = {"u_color",   *this};
    Uniform<float>   u_opacity  = {"u_opacity", *this};
};

} // namespace mbgl
