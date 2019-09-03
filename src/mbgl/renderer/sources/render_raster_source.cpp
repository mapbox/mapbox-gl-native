#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_tile.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

namespace mbgl {

using namespace style;

RenderRasterSource::RenderRasterSource(Immutable<style::RasterSource::Impl> impl_)
    : RenderTileSetSource(std::move(impl_)) {
}

inline const style::RasterSource::Impl& RenderRasterSource::impl() const {
    return static_cast<const style::RasterSource::Impl&>(*baseImpl);
}

const optional<Tileset>& RenderRasterSource::getTileset() const {
    return impl().tileset;
}

void RenderRasterSource::updateInternal(const Tileset& tileset,
                                        const std::vector<Immutable<LayerProperties>>& layers,
                                        const bool needsRendering,
                                        const bool needsRelayout,
                                        const TileParameters& parameters) {
    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::Raster,
                       impl().getTileSize(),
                       tileset.zoomRange,
                       tileset.bounds,
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<RasterTile>(tileID, parameters, tileset);
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
