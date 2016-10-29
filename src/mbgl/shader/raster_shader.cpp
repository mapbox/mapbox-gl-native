#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/raster.vertex.hpp>
#include <mbgl/shader/raster.fragment.hpp>

namespace mbgl {

RasterShader::RasterShader(gl::Context& context, Defines defines)
    : Shader(shaders::raster::name,
             shaders::raster::vertex,
             shaders::raster::fragment,
             context, defines),
      attributesState(RasterAttributes::state(*this)),
      uniformsState(RasterUniforms::state(*this)) {
}

} // namespace mbgl
