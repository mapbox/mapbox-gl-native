#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/shader/raster_uniforms.hpp>

namespace mbgl {

class RasterVertex;

class RasterShader : public gl::Shader {
public:
    RasterShader(gl::Context&, Defines defines = None);

    using VertexType = RasterVertex;
    using UniformsType = RasterUniforms;

    gl::Attribute<int16_t, 2>  a_pos         = { "a_pos",         *this };
    gl::Attribute<uint16_t, 2> a_texture_pos = { "a_texture_pos", *this };

    typename RasterUniforms::State uniformsState;
};

} // namespace mbgl
