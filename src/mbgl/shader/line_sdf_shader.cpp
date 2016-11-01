#include <mbgl/shader/line_sdf_shader.hpp>
#include <mbgl/shader/line_sdf.vertex.hpp>
#include <mbgl/shader/line_sdf.fragment.hpp>
#include <mbgl/shader/line_vertex.hpp>

namespace mbgl {

LineSDFShader::LineSDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::line_sdf::name,
             shaders::line_sdf::vertex,
             shaders::line_sdf::fragment,
             context, defines),
      uniformsState(LineSDFUniforms::state(*this)) {
}

} // namespace mbgl
