#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/collision_box_uniforms.hpp>

namespace mbgl {

class CollisionBoxVertex;

class CollisionBoxShader : public gl::Shader {
public:
    CollisionBoxShader(gl::Context&);

    using VertexType = CollisionBoxVertex;
    using UniformsType = CollisionBoxUniforms;

    gl::Attribute<int16_t, 2> a_pos     = {"a_pos",     *this};
    gl::Attribute<int16_t, 2> a_extrude = {"a_extrude", *this};
    gl::Attribute<uint8_t, 2> a_data    = {"a_data",    *this};

    typename CollisionBoxUniforms::State uniformsState;
};

} // namespace mbgl
