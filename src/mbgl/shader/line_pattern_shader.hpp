#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/line_uniforms.hpp>

namespace mbgl {

class LineVertex;

class LinePatternShader : public gl::Shader {
public:
    LinePatternShader(gl::Context&, Defines defines = None);

    using VertexType = LineVertex;
    using UniformsType = LinePatternUniforms;

    gl::Attribute<int16_t, 2> a_pos  = { "a_pos",  *this };
    gl::Attribute<uint8_t, 4> a_data = { "a_data", *this };

    typename LinePatternUniforms::State uniformsState;
};

} // namespace mbgl
