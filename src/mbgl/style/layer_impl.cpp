#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<Layer> Layer::Impl::copy(const std::string& id_,
                                         const std::string& source_) const {
    std::unique_ptr<Layer> result = clone();
    result->baseImpl->id = id_;
    result->baseImpl->source = source_;
    return result;
}

bool Layer::Impl::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

bool Layer::Impl::needsRendering(float zoom) const {
    return passes != RenderPass::None
        && visibility != VisibilityType::None
        && minZoom <= zoom
        && maxZoom >= zoom;
}

void Layer::Impl::setObserver(LayerObserver* observer_) {
    observer = observer_;
}

} // namespace style
} // namespace mbgl
