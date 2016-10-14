#include <mbgl/shader/terrain_prepare_shader.hpp>
#include <mbgl/shader/terrain_prepare.vertex.hpp>
#include <mbgl/shader/terrain_prepare.fragment.hpp>

namespace mbgl {

TerrainPrepareShader::TerrainPrepareShader(gl::Context& context, Defines defines)
    : Shader(shaders::terrain_prepare::name,
             shaders::terrain_prepare::vertex,
             shaders::terrain_prepare::fragment,
             context, defines),
      uniformsState(TerrainPrepareUniforms::state(*this)) {
}

} // namespace mbgl
