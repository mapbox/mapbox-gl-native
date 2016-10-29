#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/circle_attributes.hpp>
#include <mbgl/shader/circle_uniforms.hpp>

namespace mbgl {

class CircleShader : public gl::Shader {
public:
    CircleShader(gl::Context&, Defines defines = None);

    using AttributesType = CircleAttributes;
    using UniformsType = CircleUniforms;

    typename CircleAttributes::State attributesState;
    typename CircleUniforms::State uniformsState;
};

} // namespace mbgl
