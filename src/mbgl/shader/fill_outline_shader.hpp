#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class FillVertex;

class FillOutlineShader : public gl::Shader {
public:
    FillOutlineShader(gl::Context&, Defines defines = None);

    using VertexType = FillVertex;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    gl::UniformMatrix<4>              u_matrix         = {"u_matrix",        *this};
    gl::Uniform<Color>                u_outline_color  = {"u_outline_color", *this};
    gl::Uniform<float>                u_opacity        = {"u_opacity",       *this};
    gl::Uniform<std::array<float, 2>> u_world          = {"u_world",         *this};
};

} // namespace mbgl
