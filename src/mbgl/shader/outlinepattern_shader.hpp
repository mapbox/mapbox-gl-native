#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class PlainVertex;

class OutlinePatternShader : public Shader {
public:
    OutlinePatternShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<PlainVertex>&,
              const int8_t* offset);

    UniformMatrix<4>              u_matrix               = {"u_matrix",               *this};
    Uniform<std::array<float, 2>> u_pattern_tl_a         = {"u_pattern_tl_a",         *this};
    Uniform<std::array<float, 2>> u_pattern_br_a         = {"u_pattern_br_a",         *this};
    Uniform<std::array<float, 2>> u_pattern_tl_b         = {"u_pattern_tl_b",         *this};
    Uniform<std::array<float, 2>> u_pattern_br_b         = {"u_pattern_br_b",         *this};
    Uniform<float>                u_opacity              = {"u_opacity",              *this};
    Uniform<float>                u_mix                  = {"u_mix",                  *this};
    Uniform<float>                u_scale_a              = {"u_scale_a",              *this};
    Uniform<float>                u_scale_b              = {"u_scale_b",              *this};
    Uniform<float>                u_tile_units_to_pixels = {"u_tile_units_to_pixels", *this};
    Uniform<int32_t>              u_image                = {"u_image",                *this};
    Uniform<std::array<float, 2>> u_pattern_size_a       = {"u_pattern_size_a",       *this};
    Uniform<std::array<float, 2>> u_pattern_size_b       = {"u_pattern_size_b",       *this};
    Uniform<std::array<float, 2>> u_pixel_coord_upper    = {"u_pixel_coord_upper",    *this};
    Uniform<std::array<float, 2>> u_pixel_coord_lower    = {"u_pixel_coord_lower",    *this};
    Uniform<std::array<float, 2>> u_world                = {"u_world",                *this};
};

} // namespace mbgl
