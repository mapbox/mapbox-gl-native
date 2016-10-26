#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

class CircleVertex;

class CircleShader : public gl::Shader {
public:
    CircleShader(gl::Context&, Defines defines = None);

    using VertexType = CircleVertex;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    gl::UniformMatrix<4>              u_matrix           = {"u_matrix",           *this};
    gl::Uniform<std::array<float, 2>> u_extrude_scale    = {"u_extrude_scale",    *this};
    gl::Uniform<float>                u_devicepixelratio = {"u_devicepixelratio", *this};
    gl::Uniform<Color>                u_color            = {"u_color",            *this};
    gl::Uniform<float>                u_radius           = {"u_radius",           *this};
    gl::Uniform<float>                u_blur             = {"u_blur",             *this};
    gl::Uniform<float>                u_opacity          = {"u_opacity",          *this};
    gl::Uniform<int32_t>              u_scale_with_map   = {"u_scale_with_map",   *this};
};

} // namespace mbgl
