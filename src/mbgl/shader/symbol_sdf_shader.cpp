#include <mbgl/shader/symbol_sdf_shader.hpp>
#include <mbgl/shader/sdf.vertex.hpp>
#include <mbgl/shader/sdf.fragment.hpp>
#include <mbgl/shader/symbol_vertex.hpp>

namespace mbgl {

SymbolSDFShader::SymbolSDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::sdf::name,
             shaders::sdf::vertex,
             shaders::sdf::fragment,
             context, defines) {
}

} // namespace mbgl
