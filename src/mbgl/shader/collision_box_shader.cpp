#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/shader/collision_box.vertex.hpp>
#include <mbgl/shader/collision_box.fragment.hpp>

namespace mbgl {

CollisionBoxShader::CollisionBoxShader(gl::Context& context)
    : Shader(shaders::collision_box::name,
             shaders::collision_box::vertex,
             shaders::collision_box::fragment,
             context),
      attributesState(CollisionBoxAttributes::state(*this)),
      uniformsState(CollisionBoxUniforms::state(*this))
{
}

} // namespace mbgl
