#include <mbgl/shader/symbol_icon_shader.hpp>
#include <mbgl/shader/symbol_icon.vertex.hpp>
#include <mbgl/shader/symbol_icon.fragment.hpp>
#include <mbgl/shader/symbol_vertex.hpp>

namespace mbgl {

SymbolIconShader::SymbolIconShader(gl::Context& context, Defines defines)
    : Shader(shaders::symbol_icon::name,
             shaders::symbol_icon::vertex,
             shaders::symbol_icon::fragment,
             context, defines) {
}

} // namespace mbgl
