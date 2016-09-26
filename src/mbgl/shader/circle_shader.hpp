#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/circle_uniforms.hpp>

namespace mbgl {

class CircleVertex;

class CircleShader : public gl::Shader {
public:
    CircleShader(gl::Context&, Defines defines = None);

    using VertexType = CircleVertex;
    using UniformsType = CircleUniforms;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    typename CircleUniforms::State uniformsState;
};

} // namespace mbgl
