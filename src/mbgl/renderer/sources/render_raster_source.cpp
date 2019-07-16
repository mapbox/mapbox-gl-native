#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

namespace mbgl {

using namespace style;

RenderRasterSource::RenderRasterSource(Immutable<style::RasterSource::Impl> impl_)
    : RenderTileSource(std::move(impl_)) {
}

const style::RasterSource::Impl& RenderRasterSource::impl() const {
    return static_cast<const style::RasterSource::Impl&>(*baseImpl);
}

void RenderRasterSource::update(Immutable<style::Source::Impl> baseImpl_,
                                const std::vector<Immutable<LayerProperties>>& layers,
                                const bool needsRendering,
                                const bool needsRelayout,
                                const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    const optional<Tileset>& implTileset = impl().tileset;
    // In Continuous mode, keep the existing tiles if the new tileset is not
    // yet available, thus providing smart style transitions without flickering.
    // In other modes, allow clearing the tile pyramid first, before the early
    // return in order to avoid render tests being flaky.
    bool canUpdateTileset = implTileset || parameters.mode != MapMode::Continuous;
    if (canUpdateTileset && tileset != implTileset) {
        tileset = implTileset;

        // TODO: this removes existing buckets, and will cause flickering.
        // Should instead refresh tile data in place.
        tilePyramid.clearAll();
    }

    if (!implTileset) {
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
    algorithm::updateTileMasks(tilePyramid.getRenderedTiles());
}

void RenderRasterSource::prepare(const SourcePrepareParameters& parameters) {
    RenderTileSource::prepare(parameters);
}

std::unordered_map<std::string, std::vector<Feature>>
RenderRasterSource::queryRenderedFeatures(const ScreenLineString&,
                                          const TransformState&,
                                          const std::unordered_map<std::string, const RenderLayer*>&,
                                          const RenderedQueryOptions&,
                                          const mat4&) const {
    return std::unordered_map<std::string, std::vector<Feature>>{};
}

std::vector<Feature> RenderRasterSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}


} // namespace mbgl
