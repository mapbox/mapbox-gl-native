#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class CircleVertex;

class CircleShader : public Shader {
public:
    CircleShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<CircleVertex>&,
              const int8_t* offset);

    UniformMatrix<4>              u_matrix           = {"u_matrix",           *this};
    Uniform<std::array<float, 2>> u_extrude_scale    = {"u_extrude_scale",    *this};
    Uniform<float>                u_devicepixelratio = {"u_devicepixelratio", *this};
    Uniform<Color>                u_color            = {"u_color",            *this};
    Uniform<float>                u_radius           = {"u_radius",           *this};
    Uniform<float>                u_blur             = {"u_blur",             *this};
    Uniform<float>                u_opacity          = {"u_opacity",          *this};
    Uniform<int32_t>              u_scale_with_map   = {"u_scale_with_map",   *this};
};

} // namespace mbgl
