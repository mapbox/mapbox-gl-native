#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class PatternShader : public Shader {
public:
    PatternShader(gl::ObjectStore&);

    void bind(GLbyte *offset) final;

    UniformMatrix<4>                u_matrix               = {"u_matrix",               *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_tl_a         = {"u_pattern_tl_a",         *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_br_a         = {"u_pattern_br_a",         *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_tl_b         = {"u_pattern_tl_b",         *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_br_b         = {"u_pattern_br_b",         *this};
    Uniform<GLfloat>                u_opacity              = {"u_opacity",              *this};
    Uniform<GLfloat>                u_mix                  = {"u_mix",                  *this};
    Uniform<GLfloat>                u_scale_a              = {"u_scale_a",              *this};
    Uniform<GLfloat>                u_scale_b              = {"u_scale_b",              *this};
    Uniform<GLfloat>                u_tile_units_to_pixels = {"u_tile_units_to_pixels", *this};
    Uniform<GLint>                  u_image                = {"u_image",                *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_size_a       = {"u_pattern_size_a",       *this};
    Uniform<std::array<GLfloat, 2>> u_pattern_size_b       = {"u_pattern_size_b",       *this};
    Uniform<std::array<GLfloat, 2>> u_pixel_coord_upper    = {"u_pixel_coord_upper",    *this};
    Uniform<std::array<GLfloat, 2>> u_pixel_coord_lower    = {"u_pixel_coord_lower",    *this};
};

} // namespace mbgl
