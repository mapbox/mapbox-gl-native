#include <mbgl/renderer/sources/render_custom_geometry_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/tile/custom_geometry_tile.hpp>

namespace mbgl {

using namespace style;

RenderCustomGeometrySource::RenderCustomGeometrySource(Immutable<style::CustomGeometrySource::Impl> impl_)
    : RenderTileSource(std::move(impl_)) {
    tilePyramid.setObserver(this);
}

const style::CustomGeometrySource::Impl& RenderCustomGeometrySource::impl() const {
    return static_cast<const style::CustomGeometrySource::Impl&>(*baseImpl);
}

void RenderCustomGeometrySource::update(Immutable<style::Source::Impl> baseImpl_,
                                 const std::vector<Immutable<style::LayerProperties>>& layers,
                                 const bool needsRendering,
                                 const bool needsRelayout,
                                 const TileParameters& parameters) {
    if (baseImpl != baseImpl_) {
        std::swap(baseImpl, baseImpl_);
        tilePyramid.clearAll();
    }

    enabled = needsRendering;

    auto tileLoader = impl().getTileLoader();
    if (!tileLoader) {
        return;
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::CustomVector,
                       util::tileSize,
                       impl().getZoomRange(),
                       {},
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<CustomGeometryTile>(tileID, impl().id, parameters, impl().getTileOptions(), *tileLoader);
                       });
}

} // namespace mbgl
