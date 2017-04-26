#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/render_background_layer.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RenderLayer> BackgroundLayer::Impl::createRenderLayer() const {
    return std::make_unique<RenderBackgroundLayer>(staticImmutableCast<BackgroundLayer::Impl>(immutableFromThis()));
}

} // namespace style
} // namespace mbgl
