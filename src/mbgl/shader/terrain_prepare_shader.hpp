#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/terrain_uniforms.hpp>

namespace mbgl {

class RasterVertex;

class TerrainPrepareShader : public gl::Shader {
public:
    TerrainPrepareShader(gl::Context&, Defines defines = None);

    using VertexType = RasterVertex;
    using UniformsType = TerrainPrepareUniforms;

    gl::Attribute<int16_t, 2> a_pos = { "a_pos", *this };
    gl::Attribute<uint16_t, 2> a_texture_pos = { "a_texture_pos", *this };

    typename TerrainPrepareUniforms::State uniformsState;
};

} // namespace mbgl
