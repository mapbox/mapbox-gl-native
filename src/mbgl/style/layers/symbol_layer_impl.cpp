#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RenderLayer> SymbolLayer::Impl::createRenderLayer() const {
    return std::make_unique<RenderSymbolLayer>(*this);
}

} // namespace style
} // namespace mbgl
