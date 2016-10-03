#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

namespace mbgl {

class CollisionBoxVertex;

class CollisionBoxShader : public gl::Shader {
public:
    CollisionBoxShader(gl::Context&);

    using VertexType = CollisionBoxVertex;

    gl::Attribute<int16_t, 2> a_pos     = {"a_pos",     *this};
    gl::Attribute<int16_t, 2> a_extrude = {"a_extrude", *this};
    gl::Attribute<uint8_t, 2> a_data    = {"a_data",    *this};

    gl::UniformMatrix<4> u_matrix  = {"u_matrix",  *this};
    gl::Uniform<float>   u_scale   = {"u_scale",   *this};
    gl::Uniform<float>   u_zoom    = {"u_zoom",    *this};
    gl::Uniform<float>   u_maxzoom = {"u_maxzoom", *this};
};

} // namespace mbgl
