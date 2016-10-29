#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/shader/fill_attributes.hpp>

namespace mbgl {

class FillOutlineShader : public gl::Shader {
public:
    FillOutlineShader(gl::Context&, Defines defines = None);

    using AttributesType = FillAttributes;
    using UniformsType = FillColorUniforms;

    typename FillAttributes::State attributesState;
    typename FillColorUniforms::State uniformsState;
};

} // namespace mbgl
