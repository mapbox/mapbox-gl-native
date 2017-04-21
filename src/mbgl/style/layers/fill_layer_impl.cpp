#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/renderer/render_fill_layer.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RenderLayer> FillLayer::Impl::createRenderLayer() const {
    return std::make_unique<RenderFillLayer>(*this);
}

} // namespace style
} // namespace mbgl
