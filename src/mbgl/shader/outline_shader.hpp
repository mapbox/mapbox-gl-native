#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class PlainVertex;

class OutlineShader : public gl::Shader {
public:
    OutlineShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<PlainVertex>&,
              const int8_t* offset);

    gl::UniformMatrix<4>              u_matrix         = {"u_matrix",        *this};
    gl::Uniform<Color>                u_outline_color  = {"u_outline_color", *this};
    gl::Uniform<float>                u_opacity        = {"u_opacity",       *this};
    gl::Uniform<std::array<float, 2>> u_world          = {"u_world",         *this};
};

} // namespace mbgl
