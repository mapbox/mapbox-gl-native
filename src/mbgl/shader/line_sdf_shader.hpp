#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/line_uniforms.hpp>
#include <mbgl/shader/line_attributes.hpp>

namespace mbgl {

class LineSDFShader : public gl::Shader {
public:
    LineSDFShader(gl::Context&, Defines defines = None);

    using AttributesType = LineAttributes;
    using UniformsType = LineSDFUniforms;

    typename LineAttributes::State attributesState;
    typename LineSDFUniforms::State uniformsState;
};


} // namespace mbgl
