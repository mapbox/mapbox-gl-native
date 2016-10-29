#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/collision_box_uniforms.hpp>
#include <mbgl/shader/collision_box_attributes.hpp>

namespace mbgl {

class CollisionBoxShader : public gl::Shader {
public:
    CollisionBoxShader(gl::Context&);

    using AttributesType = CollisionBoxAttributes;
    using UniformsType = CollisionBoxUniforms;

    typename CollisionBoxAttributes::State attributesState;
    typename CollisionBoxUniforms::State uniformsState;
};

} // namespace mbgl
