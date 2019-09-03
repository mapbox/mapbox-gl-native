#include <mbgl/renderer/sources/render_raster_dem_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/raster_dem_tile.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>
#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

namespace mbgl {

using namespace style;

RenderRasterDEMSource::RenderRasterDEMSource(Immutable<style::RasterSource::Impl> impl_)
    : RenderTileSetSource(std::move(impl_)) {
}

const style::RasterSource::Impl& RenderRasterDEMSource::impl() const {
    return static_cast<const style::RasterSource::Impl&>(*baseImpl);
}

const optional<Tileset>& RenderRasterDEMSource::getTileset() const {
    return impl().tileset;
}

void RenderRasterDEMSource::updateInternal(const Tileset& tileset,
                                           const std::vector<Immutable<LayerProperties>>& layers,
                                           const bool needsRendering,
                                           const bool needsRelayout,
                                           const TileParameters& parameters) {
    tilePyramid.update(layers,
                       needsRendering,
                       needsRelayout,
                       parameters,
                       SourceType::RasterDEM,
                       impl().getTileSize(),
                       tileset.zoomRange,
                       tileset.bounds,
                       [&] (const OverscaledTileID& tileID) {
                           return std::make_unique<RasterDEMTile>(tileID, parameters, tileset);
                       });
    algorithm::updateTileMasks(tilePyramid.getRenderedTiles());
}

void RenderRasterDEMSource::onTileChanged(Tile& tile){
    auto& demtile = static_cast<RasterDEMTile&>(tile);

    std::map<DEMTileNeighbors, DEMTileNeighbors> opposites = {
        { DEMTileNeighbors::Left, DEMTileNeighbors::Right },
        { DEMTileNeighbors::Right, DEMTileNeighbors::Left },
        { DEMTileNeighbors::TopLeft, DEMTileNeighbors::BottomRight },
        { DEMTileNeighbors::TopCenter, DEMTileNeighbors::BottomCenter },
        { DEMTileNeighbors::TopRight, DEMTileNeighbors::BottomLeft },
        { DEMTileNeighbors::BottomRight,  DEMTileNeighbors::TopLeft },
        { DEMTileNeighbors::BottomCenter, DEMTileNeighbors:: TopCenter },
        { DEMTileNeighbors::BottomLeft, DEMTileNeighbors::TopRight }
    };

    if (tile.isRenderable() && demtile.neighboringTiles != DEMTileNeighbors::Complete) {
        const CanonicalTileID canonical = tile.id.canonical;
        const uint32_t dim = std::pow(2, canonical.z);
        const uint32_t px = (canonical.x - 1 + dim) % dim;
        const int pxw = canonical.x == 0 ? tile.id.wrap - 1 : tile.id.wrap;
        const uint32_t nx = (canonical.x + 1 + dim) % dim;
        const int nxw = (canonical.x + 1 == dim) ? tile.id.wrap + 1 : tile.id.wrap;

        auto getNeighbor = [&] (DEMTileNeighbors mask){
            if (mask == DEMTileNeighbors::Left){
                return OverscaledTileID(tile.id.overscaledZ, pxw, canonical.z, px, canonical.y);
            } else if (mask == DEMTileNeighbors::Right){
                return OverscaledTileID(tile.id.overscaledZ, nxw, canonical.z, nx, canonical.y);
            } else if (mask == DEMTileNeighbors::TopLeft){
                return OverscaledTileID(tile.id.overscaledZ, pxw, canonical.z, px, canonical.y - 1);
            } else if (mask == DEMTileNeighbors::TopCenter){
                return OverscaledTileID(tile.id.overscaledZ, tile.id.wrap, canonical.z, canonical.x, canonical.y - 1);
            } else if (mask == DEMTileNeighbors::TopRight){
                return OverscaledTileID(tile.id.overscaledZ, nxw, canonical.z, nx, canonical.y - 1);
            } else if (mask == DEMTileNeighbors::BottomLeft){
                return OverscaledTileID(tile.id.overscaledZ, pxw, canonical.z, px, canonical.y + 1);
            } else if (mask == DEMTileNeighbors::BottomCenter){
                return OverscaledTileID(tile.id.overscaledZ, tile.id.wrap, canonical.z, canonical.x, canonical.y + 1);
            } else if (mask == DEMTileNeighbors::BottomRight){
                return OverscaledTileID(tile.id.overscaledZ, nxw, canonical.z, nx, canonical.y + 1);
            } else{
                throw std::runtime_error("mask is not a valid tile neighbor");
            }
        };

        for (uint8_t i = 0; i < 8; i++) {
            auto mask = DEMTileNeighbors(std::pow(2,i));
            // only backfill if this neighbor has not been previously backfilled
            if ((demtile.neighboringTiles & mask) != mask) {
                OverscaledTileID neighborid = getNeighbor(mask);
                Tile* renderableNeighbor = tilePyramid.getTile(neighborid);
                if (renderableNeighbor != nullptr && renderableNeighbor->isRenderable()) {
                    auto& borderTile = static_cast<RasterDEMTile&>(*renderableNeighbor);
                    demtile.backfillBorder(borderTile, mask);

                    // if the border tile has not been backfilled by a previous instance of the main
                    // tile, backfill its corresponding neighbor as well.
                    const DEMTileNeighbors& borderMask = opposites[mask];
                    if ((borderTile.neighboringTiles & borderMask) != borderMask){
                        borderTile.backfillBorder(demtile, borderMask);
                     }
                }
            }
        }
    }
    RenderTileSource::onTileChanged(tile);
}

std::unordered_map<std::string, std::vector<Feature>>
RenderRasterDEMSource::queryRenderedFeatures(const ScreenLineString&,
                                          const TransformState&,
                                          const std::unordered_map<std::string, const RenderLayer*>&,
                                          const RenderedQueryOptions&,
                                          const mat4&) const {
    return std::unordered_map<std::string, std::vector<Feature>>{};
}

std::vector<Feature> RenderRasterDEMSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}


} // namespace mbgl
