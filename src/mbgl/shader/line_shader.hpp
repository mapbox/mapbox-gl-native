#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class LineVertex;

class LineShader : public Shader {
public:
    LineShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<LineVertex>&,
              const int8_t* offset);

    UniformMatrix<4> u_matrix              = {"u_matrix",             *this};
    Uniform<Color>   u_color               = {"u_color",              *this};
    Uniform<float>   u_opacity             = {"u_opacity",            *this};
    Uniform<float>   u_ratio               = {"u_ratio",              *this};
    Uniform<float>   u_linewidth           = {"u_linewidth",          *this};
    Uniform<float>   u_gapwidth            = {"u_gapwidth",           *this};
    Uniform<float>   u_antialiasing        = {"u_antialiasing",       *this};
    Uniform<float>   u_blur                = {"u_blur",               *this};
    Uniform<float>   u_extra               = {"u_extra",              *this};
    Uniform<float>   u_offset              = {"u_offset",             *this};
    UniformMatrix<2> u_antialiasingmatrix  = {"u_antialiasingmatrix", *this};
};


} // namespace mbgl
