#pragma once

#include <mbgl/shader/shader.hpp>
#include <mbgl/shader/uniform.hpp>

namespace mbgl {

class CollisionBoxShader : public Shader {
public:
    CollisionBoxShader(gl::Context&);

    void bind(int8_t* offset) final;

    UniformMatrix<4> u_matrix  = {"u_matrix",  *this};
    Uniform<float>   u_scale   = {"u_scale",   *this};
    Uniform<float>   u_zoom    = {"u_zoom",    *this};
    Uniform<float>   u_maxzoom = {"u_maxzoom", *this};
};

} // namespace mbgl
