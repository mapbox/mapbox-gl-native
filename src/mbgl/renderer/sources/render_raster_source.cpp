#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>

namespace mbgl {

using namespace style;

RenderRasterSource::RenderRasterSource(Immutable<style::RasterSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const style::RasterSource::Impl& RenderRasterSource::impl() const {
    return static_cast<const style::RasterSource::Impl&>(*baseImpl);
}

bool RenderRasterSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderRasterSource::update(Immutable<style::Source::Impl> baseImpl_,
                                const std::vector<Immutable<Layer::Impl>>& layers,
                                const bool needsRendering,
                                const bool needsRelayout,
                                const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    optional<Tileset> _tileset = impl().getTileset();

    if (tileset != _tileset) {
        tileset = _tileset;

        // TODO: this removes existing buckets, and will cause flickering.
        // Should instead refresh tile data in place.
        tilePyramid.tiles.clear();
        tilePyramid.renderTiles.clear();
        tilePyramid.cache.clear();
    }
    // Allow clearing the tile pyramid first, before the early return in case
    //  the new tileset is not yet available or has an error in loading
    if (!_tileset) {
        return;
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::Raster,
                       impl().getTileSize(),
                       tileset->zoomRange,
                       tileset->bounds,
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<RasterTile>(tileID, parameters, *tileset);
                       });
}

void RenderRasterSource::startRender(PaintParameters& parameters) {
    algorithm::updateTileMasks(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderRasterSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderRasterSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderRasterSource::queryRenderedFeatures(const ScreenLineString&,
                                          const TransformState&,
                                          const std::vector<const RenderLayer*>&,
                                          const RenderedQueryOptions&,
                                          const mat4&) const {
    return std::unordered_map<std::string, std::vector<Feature>> {};
}

std::vector<Feature> RenderRasterSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderRasterSource::reduceMemoryUse() {
    tilePyramid.reduceMemoryUse();
}

void RenderRasterSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
