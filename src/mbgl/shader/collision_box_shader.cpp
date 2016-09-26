#include <mbgl/shader/collision_box_shader.hpp>
#include <mbgl/shader/collisionbox.vertex.hpp>
#include <mbgl/shader/collisionbox.fragment.hpp>
#include <mbgl/shader/collision_box_vertex.hpp>

namespace mbgl {

CollisionBoxShader::CollisionBoxShader(gl::Context& context)
    : Shader(shaders::collisionbox::name,
             shaders::collisionbox::vertex,
             shaders::collisionbox::fragment,
             context) {
}

void CollisionBoxShader::bind(const gl::VertexBuffer<CollisionBoxVertex>&,
                              const int8_t* offset) {
    CollisionBoxVertex::bind(offset);
}

} // namespace mbgl
