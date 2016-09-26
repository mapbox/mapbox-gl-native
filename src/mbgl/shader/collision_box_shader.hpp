#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

namespace gl {
template <class> class VertexBuffer;
} // namespace gl

class CollisionBoxVertex;

class CollisionBoxShader : public Shader {
public:
    CollisionBoxShader(gl::Context&);

    void bind(const gl::VertexBuffer<CollisionBoxVertex>&,
              const int8_t* offset);

    UniformMatrix<4> u_matrix  = {"u_matrix",  *this};
    Uniform<float>   u_scale   = {"u_scale",   *this};
    Uniform<float>   u_zoom    = {"u_zoom",    *this};
    Uniform<float>   u_maxzoom = {"u_maxzoom", *this};
};

} // namespace mbgl
