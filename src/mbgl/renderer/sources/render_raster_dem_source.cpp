#include <mbgl/renderer/sources/render_raster_dem_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_dem_tile.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>

namespace mbgl {

using namespace style;

RenderRasterDEMSource::RenderRasterDEMSource(Immutable<style::RasterDEMSource::Impl> impl_)
    : RenderSource(impl_) {
    tilePyramid.setObserver(this);
}

const style::RasterDEMSource::Impl& RenderRasterDEMSource::impl() const {
    return static_cast<const style::RasterDEMSource::Impl&>(*baseImpl);
}

bool RenderRasterDEMSource::isLoaded() const {
    return tilePyramid.isLoaded();
}

void RenderRasterDEMSource::update(Immutable<style::Source::Impl> baseImpl_,
                                const std::vector<Immutable<Layer::Impl>>& layers,
                                const bool needsRendering,
                                const bool needsRelayout,
                                const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    optional<Tileset> tileset = impl().getTileset();

    if (!tileset) {
        return;
    }

    if (tileURLTemplates != tileset->tiles) {
        tileURLTemplates = tileset->tiles;

        // TODO: this removes existing buckets, and will cause flickering.
        // Should instead refresh tile data in place.
        tilePyramid.tiles.clear();
        tilePyramid.renderTiles.clear();
        tilePyramid.cache.clear();
    }

    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::RasterDEM,
                       impl().getTileSize(),
                       tileset->zoomRange,
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<RasterDEMTile>(tileID, parameters, *tileset);
                       });
    auto rendertiles = getRenderTiles();
    std::sort(rendertiles.begin(), rendertiles.end(),
              [](const RenderTile& a, const RenderTile& b) { return a.id < b.id; });

    for (auto& rt : rendertiles) {
        RasterDEMTile& demtile = static_cast<RasterDEMTile&>(rt.get().tile);
        if (!demtile.isBackfilled()) {
            bool lastBackfill = true;
            for (auto& pair : demtile.neighboringTiles) {
                // the current neighboring tile has not been backfilled yet
                if (!pair.second) {
                    const auto renderableNeighbor = std::find_if(rendertiles.begin(), rendertiles.end(), [&pair] (const RenderTile& rt){
                        return rt.id.canonical == pair.first.canonical;
                    });

                    if (renderableNeighbor != rendertiles.end()) {
                        const RasterDEMTile& borderTile = static_cast<RasterDEMTile&>(renderableNeighbor->get().tile);
                        int dx = borderTile.id.canonical.x - demtile.id.canonical.x;
                        const int8_t dy = borderTile.id.canonical.y - demtile.id.canonical.y;
                        const uint32_t dim = pow(2, demtile.id.canonical.z);
                        if (dx == 0 && dy == 0) continue;
                        if (std::abs(dy) > 1) continue;
                        // neighbor is in another world wrap
                        if (std::abs(dx) > 1) {
                            if (std::abs(int(dx + dim)) == 1) {
                                dx += dim;
                            } else if (std::abs(int(dx - dim)) == 1) {
                                dx -= dim;
                            }
                        }

//                        HillshadeBucket* borderBucket = borderTile.getBucket(hillshadeLayer->baseImpl)
//                        if (!borderTile.dem || !tile.dem) return;
//                        tile.dem.backfillBorder(borderTile.dem, dx, dy);
//                        if (tile.neighboringTiles && tile.neighboringTiles[borderId])
//                            tile.neighboringTiles[borderId].backfilled = true;

                        pair.second = true;
                    } else {
                        lastBackfill = false;
                    }
                }
            }
        }
    }
}

void RenderRasterDEMSource::startRender(PaintParameters& parameters) {
    algorithm::updateTileMasks(tilePyramid.getRenderTiles());
    tilePyramid.startRender(parameters);
}

void RenderRasterDEMSource::finishRender(PaintParameters& parameters) {
    tilePyramid.finishRender(parameters);
}

std::vector<std::reference_wrapper<RenderTile>> RenderRasterDEMSource::getRenderTiles() {
    return tilePyramid.getRenderTiles();
}

std::unordered_map<std::string, std::vector<Feature>>
RenderRasterDEMSource::queryRenderedFeatures(const ScreenLineString&,
                                          const TransformState&,
                                          const std::vector<const RenderLayer*>&,
                                          const RenderedQueryOptions&,
                                          const CollisionIndex& ) const {
    return std::unordered_map<std::string, std::vector<Feature>> {};
}

std::vector<Feature> RenderRasterDEMSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}

void RenderRasterDEMSource::onLowMemory() {
    tilePyramid.onLowMemory();
}

void RenderRasterDEMSource::dumpDebugLogs() const {
    tilePyramid.dumpDebugLogs();
}

} // namespace mbgl
