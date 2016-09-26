#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/symbol_uniforms.hpp>

namespace mbgl {

class SymbolVertex;

class SymbolIconShader : public gl::Shader {
public:
    SymbolIconShader(gl::Context&, Defines defines = None);

    using VertexType = SymbolVertex;
    using UniformsType = SymbolIconUniforms;

    gl::Attribute<int16_t, 2>  a_pos         = { "a_pos",         *this };
    gl::Attribute<int16_t, 2>  a_offset      = { "a_offset",      *this };
    gl::Attribute<uint16_t, 2> a_texture_pos = { "a_texture_pos", *this };
    gl::Attribute<uint8_t, 4>  a_data        = { "a_data",        *this };

    typename SymbolIconUniforms::State uniformsState;
};

} // namespace mbgl
