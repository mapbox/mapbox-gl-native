#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/symbol_uniforms.hpp>
#include <mbgl/shader/symbol_attributes.hpp>

namespace mbgl {

class SymbolSDFShader : public gl::Shader {
public:
    SymbolSDFShader(gl::Context&, Defines defines = None);

    using AttributesType = SymbolAttributes;
    using UniformsType = SymbolSDFUniforms;

    typename SymbolAttributes::State attributesState;
    typename SymbolSDFUniforms::State uniformsState;
};

} // namespace mbgl
