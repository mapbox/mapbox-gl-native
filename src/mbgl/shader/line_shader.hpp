#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/line_uniforms.hpp>
#include <mbgl/shader/line_attributes.hpp>

namespace mbgl {

class LineShader : public gl::Shader {
public:
    LineShader(gl::Context&, Defines defines = None);

    using AttributesType = LineAttributes;
    using UniformsType = LineColorUniforms;

    typename LineAttributes::State attributesState;
    typename LineColorUniforms::State uniformsState;
};


} // namespace mbgl
