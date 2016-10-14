#include <mbgl/shader/terrain_shader.hpp>
#include <mbgl/shader/terrain.vertex.hpp>
#include <mbgl/shader/terrain.fragment.hpp>
#include <mbgl/shader/raster_vertex.hpp>

namespace mbgl {

TerrainShader::TerrainShader(gl::Context& context, Defines defines)
    : Shader(shaders::terrain::name,
             shaders::terrain::vertex,
             shaders::terrain::fragment,
             context, defines),
      uniformsState(TerrainUniforms::state(*this)) {
}

} // namespace mbgl
