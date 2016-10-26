#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

namespace mbgl {

class FillVertex;

class FillOutlinePatternShader : public gl::Shader {
public:
    FillOutlinePatternShader(gl::Context&, Defines defines = None);

    using VertexType = FillVertex;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    gl::UniformMatrix<4>              u_matrix               = {"u_matrix",               *this};
    gl::Uniform<std::array<float, 2>> u_pattern_tl_a         = {"u_pattern_tl_a",         *this};
    gl::Uniform<std::array<float, 2>> u_pattern_br_a         = {"u_pattern_br_a",         *this};
    gl::Uniform<std::array<float, 2>> u_pattern_tl_b         = {"u_pattern_tl_b",         *this};
    gl::Uniform<std::array<float, 2>> u_pattern_br_b         = {"u_pattern_br_b",         *this};
    gl::Uniform<float>                u_opacity              = {"u_opacity",              *this};
    gl::Uniform<float>                u_mix                  = {"u_mix",                  *this};
    gl::Uniform<float>                u_scale_a              = {"u_scale_a",              *this};
    gl::Uniform<float>                u_scale_b              = {"u_scale_b",              *this};
    gl::Uniform<float>                u_tile_units_to_pixels = {"u_tile_units_to_pixels", *this};
    gl::Uniform<int32_t>              u_image                = {"u_image",                *this};
    gl::Uniform<std::array<float, 2>> u_pattern_size_a       = {"u_pattern_size_a",       *this};
    gl::Uniform<std::array<float, 2>> u_pattern_size_b       = {"u_pattern_size_b",       *this};
    gl::Uniform<std::array<float, 2>> u_pixel_coord_upper    = {"u_pixel_coord_upper",    *this};
    gl::Uniform<std::array<float, 2>> u_pixel_coord_lower    = {"u_pixel_coord_lower",    *this};
    gl::Uniform<std::array<float, 2>> u_world                = {"u_world",                *this};
};

} // namespace mbgl
