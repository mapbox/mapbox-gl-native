#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class LineVertex;

class LinepatternShader : public Shader {
public:
    LinepatternShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<LineVertex>&,
              const int8_t* offset);

    UniformMatrix<4>              u_matrix              = {"u_matrix",              *this};
    Uniform<float>                u_linewidth           = {"u_linewidth",           *this};
    Uniform<float>                u_gapwidth            = {"u_gapwidth",            *this};
    Uniform<float>                u_antialiasing        = {"u_antialiasing",        *this};
    Uniform<std::array<float, 2>> u_pattern_size_a      = {"u_pattern_size_a",      *this};
    Uniform<std::array<float, 2>> u_pattern_tl_a        = {"u_pattern_tl_a",        *this};
    Uniform<std::array<float, 2>> u_pattern_br_a        = {"u_pattern_br_a",        *this};
    Uniform<std::array<float, 2>> u_pattern_size_b      = {"u_pattern_size_b",      *this};
    Uniform<std::array<float, 2>> u_pattern_tl_b        = {"u_pattern_tl_b",        *this};
    Uniform<std::array<float, 2>> u_pattern_br_b        = {"u_pattern_br_b",        *this};
    Uniform<float>                u_ratio               = {"u_ratio",               *this};
    Uniform<float>                u_point               = {"u_point",               *this};
    Uniform<float>                u_blur                = {"u_blur",                *this};
    Uniform<float>                u_fade                = {"u_fade",                *this};
    Uniform<float>                u_opacity             = {"u_opacity",             *this};
    Uniform<float>                u_extra               = {"u_extra",               *this};
    Uniform<float>                u_offset              = {"u_offset",              *this};
    Uniform<int32_t>              u_image               = {"u_image",               *this};
    UniformMatrix<2>              u_antialiasingmatrix  = {"u_antialiasingmatrix",  *this};
};

} // namespace mbgl
