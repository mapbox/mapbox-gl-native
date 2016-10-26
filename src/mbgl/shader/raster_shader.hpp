#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

namespace mbgl {

class RasterVertex;

class RasterShader : public gl::Shader {
public:
    RasterShader(gl::Context&, Defines defines = None);

    using VertexType = RasterVertex;

    gl::Attribute<int16_t, 2>  a_pos         = { "a_pos",         *this };
    gl::Attribute<uint16_t, 2> a_texture_pos = { "a_texture_pos", *this };

    gl::UniformMatrix<4>              u_matrix            = {"u_matrix",            *this};
    gl::Uniform<int32_t>              u_image0            = {"u_image0",            *this};
    gl::Uniform<int32_t>              u_image1            = {"u_image1",            *this};
    gl::Uniform<float>                u_opacity0          = {"u_opacity0",          *this};
    gl::Uniform<float>                u_opacity1          = {"u_opacity1",          *this};
    gl::Uniform<float>                u_buffer_scale      = {"u_buffer_scale",      *this};
    gl::Uniform<float>                u_brightness_low    = {"u_brightness_low",    *this};
    gl::Uniform<float>                u_brightness_high   = {"u_brightness_high",   *this};
    gl::Uniform<float>                u_saturation_factor = {"u_saturation_factor", *this};
    gl::Uniform<float>                u_contrast_factor   = {"u_contrast_factor",   *this};
    gl::Uniform<std::array<float, 3>> u_spin_weights      = {"u_spin_weights",      *this};
    gl::Uniform<std::array<float, 2>> u_tl_parent         = {"u_tl_parent",         *this};
    gl::Uniform<float>                u_scale_parent      = {"u_scale_parent",      *this};
};

} // namespace mbgl
