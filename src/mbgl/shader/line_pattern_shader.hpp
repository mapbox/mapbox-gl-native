#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/attribute.hpp>

namespace mbgl {

class LineVertex;

class LinePatternShader : public gl::Shader {
public:
    LinePatternShader(gl::Context&, Defines defines = None);

    using VertexType = LineVertex;

    gl::Attribute<int16_t, 2> a_pos  = { "a_pos",  *this };
    gl::Attribute<uint8_t, 4> a_data = { "a_data", *this };

    gl::UniformMatrix<4>              u_matrix              = {"u_matrix",              *this};
    gl::Uniform<float>                u_linewidth           = {"u_linewidth",           *this};
    gl::Uniform<float>                u_gapwidth            = {"u_gapwidth",            *this};
    gl::Uniform<float>                u_antialiasing        = {"u_antialiasing",        *this};
    gl::Uniform<std::array<float, 2>> u_pattern_size_a      = {"u_pattern_size_a",      *this};
    gl::Uniform<std::array<float, 2>> u_pattern_tl_a        = {"u_pattern_tl_a",        *this};
    gl::Uniform<std::array<float, 2>> u_pattern_br_a        = {"u_pattern_br_a",        *this};
    gl::Uniform<std::array<float, 2>> u_pattern_size_b      = {"u_pattern_size_b",      *this};
    gl::Uniform<std::array<float, 2>> u_pattern_tl_b        = {"u_pattern_tl_b",        *this};
    gl::Uniform<std::array<float, 2>> u_pattern_br_b        = {"u_pattern_br_b",        *this};
    gl::Uniform<float>                u_ratio               = {"u_ratio",               *this};
    gl::Uniform<float>                u_point               = {"u_point",               *this};
    gl::Uniform<float>                u_blur                = {"u_blur",                *this};
    gl::Uniform<float>                u_fade                = {"u_fade",                *this};
    gl::Uniform<float>                u_opacity             = {"u_opacity",             *this};
    gl::Uniform<float>                u_extra               = {"u_extra",               *this};
    gl::Uniform<float>                u_offset              = {"u_offset",              *this};
    gl::Uniform<int32_t>              u_image               = {"u_image",               *this};
    gl::UniformMatrix<2>              u_antialiasingmatrix  = {"u_antialiasingmatrix",  *this};
};

} // namespace mbgl
