#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/renderer/render_line_layer.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<RenderLayer> LineLayer::Impl::createRenderLayer() const {
    return std::make_unique<RenderLineLayer>(staticImmutableCast<LineLayer::Impl>(immutableFromThis()));
}

} // namespace style
} // namespace mbgl
