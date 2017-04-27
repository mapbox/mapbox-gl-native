#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_tile.hpp>

namespace mbgl {

using namespace style;

RenderRasterSource::RenderRasterSource(const style::RasterSource::Impl& impl_)
    : RenderSource(impl_),
      impl(impl_) {
    tilePyramid.setObserver(this);
}

bool RenderRasterSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderRasterSource::invalidateTiles() {
    tilePyramid.invalidateTiles();
}

void RenderRasterSource::startRender(algorithm::ClipIDGenerator&, const mat4& projMatrix, const mat4& clipMatrix, const TransformState& transform) {
    tilePyramid.startRender(projMatrix, clipMatrix, transform);
}

void RenderRasterSource::finishRender(Painter& painter) {
    tilePyramid.finishRender(painter);
}

std::map<UnwrappedTileID, RenderTile>& RenderRasterSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

void RenderRasterSource::updateTiles(const TileParameters& parameters) {
    optional<Tileset> tileset = impl.getTileset();

    if (!tileset) {
        return;
    }

    if (tileURLTemplates != tileset->tiles) {
        tileURLTemplates = tileset->tiles;
        tilePyramid.invalidateTiles();
    }

    tilePyramid.updateTiles(parameters,
                            SourceType::Raster,
                            impl.getTileSize(),
                            tileset->zoomRange,
                            [&] (const OverscaledTileID& tileID) {
                                return std::make_unique<RasterTile>(tileID, parameters, *tileset);
                            });
}

void RenderRasterSource::removeTiles() {
    tilePyramid.removeTiles();
}

void RenderRasterSource::reloadTiles() {
    tilePyramid.reloadTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderRasterSource::queryRenderedFeatures(const ScreenLineString&,
                                          const TransformState&,
                                          const RenderedQueryOptions&) const {
    return {};
}

std::vector<Feature> RenderRasterSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderRasterSource::setCacheSize(size_t size) {
    tilePyramid.setCacheSize(size);
}

void RenderRasterSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderRasterSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
