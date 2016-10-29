#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/line_uniforms.hpp>
#include <mbgl/shader/line_attributes.hpp>

namespace mbgl {

class LinePatternShader : public gl::Shader {
public:
    LinePatternShader(gl::Context&, Defines defines = None);

    using AttributesType = LineAttributes;
    using UniformsType = LinePatternUniforms;

    typename LineAttributes::State attributesState;
    typename LinePatternUniforms::State uniformsState;
};

} // namespace mbgl
