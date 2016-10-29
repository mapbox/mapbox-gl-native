#include <mbgl/shader/symbol_sdf_shader.hpp>
#include <mbgl/shader/symbol_sdf.vertex.hpp>
#include <mbgl/shader/symbol_sdf.fragment.hpp>

namespace mbgl {

SymbolSDFShader::SymbolSDFShader(gl::Context& context, Defines defines)
    : Shader(shaders::symbol_sdf::name,
             shaders::symbol_sdf::vertex,
             shaders::symbol_sdf::fragment,
             context, defines),
      attributesState(SymbolAttributes::state(*this)),
      uniformsState(SymbolSDFUniforms::state(*this)) {
}

} // namespace mbgl
