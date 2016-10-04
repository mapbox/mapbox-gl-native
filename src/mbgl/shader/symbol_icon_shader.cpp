#include <mbgl/shader/symbol_icon_shader.hpp>
#include <mbgl/shader/icon.vertex.hpp>
#include <mbgl/shader/icon.fragment.hpp>
#include <mbgl/shader/symbol_vertex.hpp>

namespace mbgl {

SymbolIconShader::SymbolIconShader(gl::Context& context, Defines defines)
    : Shader(shaders::icon::name,
             shaders::icon::vertex,
             shaders::icon::fragment,
             context, defines) {
}

} // namespace mbgl
