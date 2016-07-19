#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class RasterShader : public Shader {
public:
    RasterShader(gl::ObjectStore&, Defines defines = None);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix            = {"u_matrix",            *this};
    Uniform<GLint>                  u_image0            = {"u_image0",            *this};
    Uniform<GLint>                  u_image1            = {"u_image1",            *this};
    Uniform<GLfloat>                u_opacity0          = {"u_opacity0",          *this};
    Uniform<GLfloat>                u_opacity1          = {"u_opacity1",          *this};
    Uniform<GLfloat>                u_buffer_scale      = {"u_buffer_scale",      *this};
    Uniform<GLfloat>                u_brightness_low    = {"u_brightness_low",    *this};
    Uniform<GLfloat>                u_brightness_high   = {"u_brightness_high",   *this};
    Uniform<GLfloat>                u_saturation_factor = {"u_saturation_factor", *this};
    Uniform<GLfloat>                u_contrast_factor   = {"u_contrast_factor",   *this};
    Uniform<std::array<GLfloat, 3>> u_spin_weights      = {"u_spin_weights",      *this};
    Uniform<std::array<GLfloat, 2>> u_tl_parent         = {"u_tl_parent",         *this};
    Uniform<GLfloat>                u_scale_parent      = {"u_scale_parent",      *this};
};

} // namespace mbgl
