#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class CollisionBoxVertex;

class CollisionBoxShader : public gl::Shader {
public:
    CollisionBoxShader(gl::Context&);

    void bind(const gl::VertexBuffer<CollisionBoxVertex>&,
              const int8_t* offset);

    gl::UniformMatrix<4> u_matrix  = {"u_matrix",  *this};
    gl::Uniform<float>   u_scale   = {"u_scale",   *this};
    gl::Uniform<float>   u_zoom    = {"u_zoom",    *this};
    gl::Uniform<float>   u_maxzoom = {"u_maxzoom", *this};
};

} // namespace mbgl
