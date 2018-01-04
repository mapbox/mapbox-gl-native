#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/renderer/render_tile.hpp>

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
    case LayerType::Background:
        return std::make_unique<RenderBackgroundLayer>(staticImmutableCast<BackgroundLayer::Impl>(impl));
    case LayerType::Custom:
        return std::make_unique<RenderCustomLayer>(staticImmutableCast<CustomLayer::Impl>(impl));
    case LayerType::FillExtrusion:
        return std::make_unique<RenderFillExtrusionLayer>(staticImmutableCast<FillExtrusionLayer::Impl>(impl));
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

void RenderLayer::setRenderTiles(std::vector<std::reference_wrapper<RenderTile>> tiles) {
    renderTiles = std::move(tiles);
}

} //namespace mbgl

