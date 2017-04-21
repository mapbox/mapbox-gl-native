#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {

RenderLayer::RenderLayer(style::LayerType type_, const style::Layer::Impl& baseImpl_)
        : type(type_), baseImpl(baseImpl_) {
}

const std::string& RenderLayer::getID() const {
    return baseImpl.id;
}

bool RenderLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

bool RenderLayer::needsRendering(float zoom) const {
    return passes != RenderPass::None
           && baseImpl.visibility != style::VisibilityType::None
           && baseImpl.minZoom <= zoom
           && baseImpl.maxZoom >= zoom;
}

}