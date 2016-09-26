#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class RasterVertex;

class RasterShader : public Shader {
public:
    RasterShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<RasterVertex>&,
              const int8_t* offset);

    UniformMatrix<4>              u_matrix            = {"u_matrix",            *this};
    Uniform<int32_t>              u_image0            = {"u_image0",            *this};
    Uniform<int32_t>              u_image1            = {"u_image1",            *this};
    Uniform<float>                u_opacity0          = {"u_opacity0",          *this};
    Uniform<float>                u_opacity1          = {"u_opacity1",          *this};
    Uniform<float>                u_buffer_scale      = {"u_buffer_scale",      *this};
    Uniform<float>                u_brightness_low    = {"u_brightness_low",    *this};
    Uniform<float>                u_brightness_high   = {"u_brightness_high",   *this};
    Uniform<float>                u_saturation_factor = {"u_saturation_factor", *this};
    Uniform<float>                u_contrast_factor   = {"u_contrast_factor",   *this};
    Uniform<std::array<float, 3>> u_spin_weights      = {"u_spin_weights",      *this};
    Uniform<std::array<float, 2>> u_tl_parent         = {"u_tl_parent",         *this};
    Uniform<float>                u_scale_parent      = {"u_scale_parent",      *this};
};

} // namespace mbgl
