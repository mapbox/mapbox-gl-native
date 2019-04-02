#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

RenderLayer::RenderLayer(Immutable<style::Layer::Impl> baseImpl_)
    : baseImpl(std::move(baseImpl_)) {
}

void RenderLayer::setImpl(Immutable<style::Layer::Impl> impl) {
    baseImpl = std::move(impl);
}

const std::string& RenderLayer::getID() const {
    return baseImpl->id;
}

bool RenderLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

bool RenderLayer::needsRendering(float zoom) const {
    return passes != RenderPass::None
           && baseImpl->visibility != style::VisibilityType::None
           && baseImpl->minZoom <= zoom
           && baseImpl->maxZoom >= zoom;
}

void RenderLayer::setRenderTiles(RenderTiles tiles, const TransformState&) {
    auto filterFn = [](auto& tile){ return !tile.tile.isRenderable() || tile.tile.holdForFade(); };
    renderTiles = filterRenderTiles(std::move(tiles), filterFn);
}

const RenderLayerSymbolInterface* RenderLayer::getSymbolInterface() const {
    return nullptr;
}

void RenderLayer::update() {
    // no-op
}

optional<Color> RenderLayer::getSolidBackground() const {
    return nullopt;
}

RenderLayer::RenderTiles RenderLayer::filterRenderTiles(RenderTiles tiles, FilterFunctionPtr filterFn) const {
    assert(filterFn != nullptr);
    RenderTiles filtered;
    // We only need clipping when we're drawing fill or line layers.
    const bool needsClipping_ =
            baseImpl->getTypeInfo()->clipping == LayerTypeInfo::Clipping::Required;

    for (auto& tileRef : tiles) {
        auto& tile = tileRef.get();
        if (filterFn(tile)) {
            continue;
        }

        if (Bucket* bucket = tile.tile.getBucket(*baseImpl)) {
            tile.used = true;
            tile.needsClipping |= needsClipping_;
            filtered.emplace_back(tile);
            if (tile.tile.isComplete()) {
                updateBucketPaintProperties(bucket);
            }
        }
    }
    return filtered;
}

void RenderLayer::markContextDestroyed() {
    // no-op
}

void RenderLayer::updateBucketPaintProperties(Bucket*) const {
    // no-op
}

void RenderLayer::checkRenderability(const PaintParameters& parameters,
                                     const uint32_t activeBindingCount) {
    // Only warn once for every layer.
    if (hasRenderFailures) {
        return;
    }

    if (activeBindingCount > parameters.context.maximumVertexBindingCount) {
        Log::Error(Event::OpenGL,
                   "The layer '%s' uses more data-driven properties than the current device "
                   "supports, and will have rendering errors. To ensure compatibility with this "
                   "device, use %d fewer data driven properties in this layer.",
                   getID().c_str(),
                   activeBindingCount - parameters.context.minimumRequiredVertexBindingCount);
        hasRenderFailures = true;
    } else if (activeBindingCount > parameters.context.minimumRequiredVertexBindingCount) {
        Log::Warning(Event::OpenGL,
                   "The layer '%s' uses more data-driven properties than some devices may support. "
                   "Though it will render correctly on this device, it may have rendering errors "
                   "on other devices. To ensure compatibility with all devices, use %d fewer "
                   "data-driven properties in this layer.",
                   getID().c_str(),
                   activeBindingCount - parameters.context.minimumRequiredVertexBindingCount);
        hasRenderFailures = true;
    }
}

} //namespace mbgl

