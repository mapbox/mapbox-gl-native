#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/fill_uniforms.hpp>

namespace mbgl {

class FillVertex;

class FillShader : public gl::Shader {
public:
    FillShader(gl::Context&, Defines defines = None);

    using VertexType = FillVertex;
    using UniformsType = FillColorUniforms;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    typename FillColorUniforms::State uniformsState;
};

} // namespace mbgl
