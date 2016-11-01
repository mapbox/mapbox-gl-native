#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/line_uniforms.hpp>

namespace mbgl {

class LineVertex;

class LineSDFShader : public gl::Shader {
public:
    LineSDFShader(gl::Context&, Defines defines = None);

    using VertexType = LineVertex;
    using UniformsType = LineSDFUniforms;

    gl::Attribute<int16_t, 2> a_pos  = { "a_pos",  *this };
    gl::Attribute<uint8_t, 4> a_data = { "a_data", *this };

    typename LineSDFUniforms::State uniformsState;
};


} // namespace mbgl
