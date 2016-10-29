#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/symbol_uniforms.hpp>
#include <mbgl/shader/symbol_attributes.hpp>

namespace mbgl {

class SymbolIconShader : public gl::Shader {
public:
    SymbolIconShader(gl::Context&, Defines defines = None);

    using AttributesType = SymbolAttributes;
    using UniformsType = SymbolIconUniforms;

    typename SymbolAttributes::State attributesState;
    typename SymbolIconUniforms::State uniformsState;
};

} // namespace mbgl
