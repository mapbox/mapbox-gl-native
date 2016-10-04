#include <mbgl/shader/symbol_vertex.hpp>
#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

static_assert(sizeof(SymbolVertex) == 16, "expected SymbolVertex size");

} // namespace mbgl
