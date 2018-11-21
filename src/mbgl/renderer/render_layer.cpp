#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_hillshade_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

std::unique_ptr<RenderLayer> RenderLayer::create(Immutable<Layer::Impl> impl) {
    switch (impl->type) {
    case LayerType::Fill:
        return std::make_unique<RenderFillLayer>(staticImmutableCast<FillLayer::Impl>(impl));
    case LayerType::Line:
        return std::make_unique<RenderLineLayer>(staticImmutableCast<LineLayer::Impl>(impl));
    case LayerType::Circle:
        return std::make_unique<RenderCircleLayer>(staticImmutableCast<CircleLayer::Impl>(impl));
    case LayerType::Symbol:
        return std::make_unique<RenderSymbolLayer>(staticImmutableCast<SymbolLayer::Impl>(impl));
    case LayerType::Raster:
        return std::make_unique<RenderRasterLayer>(staticImmutableCast<RasterLayer::Impl>(impl));
    case LayerType::Hillshade:
        return std::make_unique<RenderHillshadeLayer>(staticImmutableCast<HillshadeLayer::Impl>(impl));
    case LayerType::Background:
        return std::make_unique<RenderBackgroundLayer>(staticImmutableCast<BackgroundLayer::Impl>(impl));
    case LayerType::Custom:
        return std::make_unique<RenderCustomLayer>(staticImmutableCast<CustomLayer::Impl>(impl));
    case LayerType::FillExtrusion:
        return std::make_unique<RenderFillExtrusionLayer>(staticImmutableCast<FillExtrusionLayer::Impl>(impl));
    case LayerType::Heatmap:
        return std::make_unique<RenderHeatmapLayer>(staticImmutableCast<HeatmapLayer::Impl>(impl));
    }

    // Not reachable, but placate GCC.
    assert(false);
    return nullptr;
}

RenderLayer::RenderLayer(style::LayerType type_, Immutable<style::Layer::Impl> baseImpl_)
        : type(type_),
          baseImpl(baseImpl_) {
}

void RenderLayer::setImpl(Immutable<style::Layer::Impl> impl) {
    baseImpl = impl;
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

void RenderLayer::setRenderTiles(RenderTiles tiles, const TransformState& state) {
    renderTiles = filterRenderTiles(std::move(tiles));
    sortRenderTiles(state);
}

RenderLayer::RenderTiles RenderLayer::filterRenderTiles(RenderTiles tiles) const {
    auto filterFn = [](auto& tile){ return !tile.tile.isRenderable() || tile.tile.holdForFade(); };
    return filterRenderTiles(std::move(tiles), filterFn);
}

void RenderLayer::sortRenderTiles(const TransformState&) {
    std::sort(renderTiles.begin(), renderTiles.end(), [](const auto& a, const auto& b) { return a.get().id < b.get().id; });
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

        if (tile.tile.getBucket(*baseImpl)) {
            tile.used = true;
            tile.needsClipping |= needsClipping_;
            filtered.emplace_back(tile);
        }
    }
    return filtered;
}

void RenderLayer::markContextDestroyed() {
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

