#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/shader/fill_attributes.hpp>

namespace mbgl {

class FillShader : public gl::Shader {
public:
    FillShader(gl::Context&, Defines defines = None);

    using AttributesType = FillAttributes;
    using UniformsType = FillColorUniforms;

    typename FillAttributes::State attributesState;
    typename FillColorUniforms::State uniformsState;
};

} // namespace mbgl
