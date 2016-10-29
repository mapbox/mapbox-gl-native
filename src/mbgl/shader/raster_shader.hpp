#pragma once

#include <mbgl/gl/shader.hpp>
#include <mbgl/shader/raster_uniforms.hpp>
#include <mbgl/shader/raster_attributes.hpp>

namespace mbgl {

class RasterShader : public gl::Shader {
public:
    RasterShader(gl::Context&, Defines defines = None);

    using AttributesType = RasterAttributes;
    using UniformsType = RasterUniforms;

    typename RasterAttributes::State attributesState;
    typename RasterUniforms::State uniformsState;
};

} // namespace mbgl
