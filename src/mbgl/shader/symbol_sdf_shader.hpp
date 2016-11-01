#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/symbol_uniforms.hpp>

namespace mbgl {

class SymbolVertex;

class SymbolSDFShader : public gl::Shader {
public:
    SymbolSDFShader(gl::Context&, Defines defines = None);

    using VertexType = SymbolVertex;
    using UniformsType = SymbolSDFUniforms;

    gl::Attribute<int16_t, 2>  a_pos         = { "a_pos",         *this };
    gl::Attribute<int16_t, 2>  a_offset      = { "a_offset",      *this };
    gl::Attribute<uint16_t, 2> a_texture_pos = { "a_texture_pos", *this };
    gl::Attribute<uint8_t, 4>  a_data        = { "a_data",        *this };

    typename SymbolSDFUniforms::State uniformsState;
};

} // namespace mbgl
