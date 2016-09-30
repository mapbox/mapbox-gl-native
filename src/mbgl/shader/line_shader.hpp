#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class LineVertex;

class LineShader : public gl::Shader {
public:
    LineShader(gl::Context&, Defines defines = None);

    void bind(const gl::VertexBuffer<LineVertex>&,
              const int8_t* offset);

    gl::UniformMatrix<4> u_matrix              = {"u_matrix",             *this};
    gl::Uniform<Color>   u_color               = {"u_color",              *this};
    gl::Uniform<float>   u_opacity             = {"u_opacity",            *this};
    gl::Uniform<float>   u_ratio               = {"u_ratio",              *this};
    gl::Uniform<float>   u_linewidth           = {"u_linewidth",          *this};
    gl::Uniform<float>   u_gapwidth            = {"u_gapwidth",           *this};
    gl::Uniform<float>   u_antialiasing        = {"u_antialiasing",       *this};
    gl::Uniform<float>   u_blur                = {"u_blur",               *this};
    gl::Uniform<float>   u_extra               = {"u_extra",              *this};
    gl::Uniform<float>   u_offset              = {"u_offset",             *this};
    gl::UniformMatrix<2> u_antialiasingmatrix  = {"u_antialiasingmatrix", *this};
};


} // namespace mbgl
