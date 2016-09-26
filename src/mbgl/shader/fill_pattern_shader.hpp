#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/fill_uniforms.hpp>

namespace mbgl {

class FillVertex;

class FillPatternShader : public gl::Shader {
public:
    FillPatternShader(gl::Context&, Defines defines = None);

    using VertexType = FillVertex;
    using UniformsType = FillPatternUniforms;

    gl::Attribute<int16_t, 2> a_pos = {"a_pos", *this};

    typename FillPatternUniforms::State uniformsState;
};

} // namespace mbgl
