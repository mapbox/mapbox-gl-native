#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/algorithm/update_renderables.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <algorithm>

namespace mbgl {

using namespace style;

static TileObserver nullObserver;

TilePyramid::TilePyramid()
    : observer(&nullObserver) {
}

TilePyramid::~TilePyramid() = default;

bool TilePyramid::isLoaded() const {
    for (const auto& pair : tiles) {
        if (!pair.second->isComplete()) {
            return false;
        }
    }

    return true;
}

void TilePyramid::startRender(Painter& painter) {
    for (auto& tile : renderTiles) {
        tile.startRender(painter);
    }
}

void TilePyramid::finishRender(Painter& painter) {
    for (auto& tile : renderTiles) {
        tile.finishRender(painter);
    }
}

std::vector<std::reference_wrapper<RenderTile>> TilePyramid::getRenderTiles() {
    return { renderTiles.begin(), renderTiles.end() };
}

void TilePyramid::update(const std::vector<Immutable<style::Layer::Impl>>& layers,
                         const bool needsRendering,
                         const bool needsRelayout,
                         const TileParameters& parameters,
                         const SourceType type,
                         const uint16_t tileSize,
                         const Range<uint8_t> zoomRange,
                         std::function<std::unique_ptr<Tile> (const OverscaledTileID&)> createTile) {
    // If we need a relayout, abandon any cached tiles; they're now stale.
    if (needsRelayout) {
        cache.clear();
    }

    // If we're not going to render anything, move our existing tiles into
    // the cache (if they're not stale) or abandon them, and return.
    if (!needsRendering) {
        if (!needsRelayout) {
            for (auto& entry : tiles) {
                cache.add(entry.first, std::move(entry.second));
            }
        }

        tiles.clear();
        renderTiles.clear();

        return;
    }

    // Determine the overzooming/underzooming amounts and required tiles.
    int32_t overscaledZoom = util::coveringZoomLevel(parameters.transformState.getZoom(), type, tileSize);
    int32_t tileZoom = overscaledZoom;
    int32_t panZoom = zoomRange.max;

    std::vector<UnwrappedTileID> idealTiles;
    std::vector<UnwrappedTileID> panTiles;

    if (overscaledZoom >= zoomRange.min) {
        int32_t idealZoom = std::min<int32_t>(zoomRange.max, overscaledZoom);

        // Make sure we're not reparsing overzoomed raster tiles.
        if (type == SourceType::Raster) {
            tileZoom = idealZoom;

            // FIXME: Prefetching is only enabled for raster
            // tiles until we fix #7026.

            // Request lower zoom level tiles (if configure to do so) in an attempt
            // to show something on the screen faster at the cost of a little of bandwidth.
            if (parameters.prefetchZoomDelta) {
                panZoom = std::max<int32_t>(tileZoom - parameters.prefetchZoomDelta, zoomRange.min);
            }

            if (panZoom < tileZoom) {
                panTiles = util::tileCover(parameters.transformState, panZoom);
            }
        }

        idealTiles = util::tileCover(parameters.transformState, idealZoom);
    }

    // Stores a list of all the tiles that we're definitely going to retain. There are two
    // kinds of tiles we need: the ideal tiles determined by the tile cover. They may not yet be in
    // use because they're still loading. In addition to that, we also need to retain all tiles that
    // we're actively using, e.g. as a replacement for tile that aren't loaded yet.
    std::set<OverscaledTileID> retain;

    auto retainTileFn = [&](Tile& tile, Resource::Necessity necessity) -> void {
        if (retain.emplace(tile.id).second) {
            tile.setNecessity(necessity);
        }

        if (needsRelayout) {
            tile.setLayers(layers);
        }
    };
    auto getTileFn = [&](const OverscaledTileID& tileID) -> Tile* {
        auto it = tiles.find(tileID);
        return it == tiles.end() ? nullptr : it->second.get();
    };
    auto createTileFn = [&](const OverscaledTileID& tileID) -> Tile* {
        std::unique_ptr<Tile> tile = cache.get(tileID);
        if (!tile) {
            tile = createTile(tileID);
            if (tile) {
                tile->setObserver(observer);
                tile->setLayers(layers);
            }
        }
        if (!tile) {
            return nullptr;
        }
        return tiles.emplace(tileID, std::move(tile)).first->second.get();
    };
    auto renderTileFn = [&](const UnwrappedTileID& tileID, Tile& tile) {
        renderTiles.emplace_back(tileID, tile);
    };

    renderTiles.clear();

    if (!panTiles.empty()) {
        algorithm::updateRenderables(getTileFn, createTileFn, retainTileFn,
                [](const UnwrappedTileID&, Tile&) {}, panTiles, zoomRange, panZoom);
    }

    algorithm::updateRenderables(getTileFn, createTileFn, retainTileFn, renderTileFn,
                                 idealTiles, zoomRange, tileZoom);

    if (type != SourceType::Annotations) {
        size_t conservativeCacheSize =
            std::max((float)parameters.transformState.getSize().width / tileSize, 1.0f) *
            std::max((float)parameters.transformState.getSize().height / tileSize, 1.0f) *
            (parameters.transformState.getMaxZoom() - parameters.transformState.getMinZoom() + 1) *
            0.5;
        cache.setSize(conservativeCacheSize);
    }

    removeStaleTiles(retain);

    for (auto& pair : tiles) {
        const PlacementConfig config { parameters.transformState.getAngle(),
                                       parameters.transformState.getPitch(),
                                       parameters.transformState.getCameraToCenterDistance(),
                                       parameters.transformState.getCameraToTileDistance(pair.first.toUnwrapped()),
                                       parameters.debugOptions & MapDebugOptions::Collision };

        pair.second->setPlacementConfig(config);
    }
}

// Moves all tiles to the cache except for those specified in the retain set.
void TilePyramid::removeStaleTiles(const std::set<OverscaledTileID>& retain) {
    // Remove stale tiles. This goes through the (sorted!) tiles map and retain set in lockstep
    // and removes items from tiles that don't have the corresponding key in the retain set.
    auto tilesIt = tiles.begin();
    auto retainIt = retain.begin();
    while (tilesIt != tiles.end()) {
        if (retainIt == retain.end() || tilesIt->first < *retainIt) {
            tilesIt->second->setNecessity(Tile::Necessity::Optional);
            cache.add(tilesIt->first, std::move(tilesIt->second));
            tiles.erase(tilesIt++);
        } else {
            if (!(*retainIt < tilesIt->first)) {
                ++tilesIt;
            }
            ++retainIt;
        }
    }
}

std::unordered_map<std::string, std::vector<Feature>> TilePyramid::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const RenderStyle& style,
                                           const RenderedQueryOptions& options) const {
    std::unordered_map<std::string, std::vector<Feature>> result;
    if (renderTiles.empty() || geometry.empty()) {
        return result;
    }

    LineString<double> queryGeometry;

    for (const auto& p : geometry) {
        queryGeometry.push_back(TileCoordinate::fromScreenCoordinate(
            transformState, 0, { p.x, transformState.getSize().height - p.y }).p);
    }

    mapbox::geometry::box<double> box = mapbox::geometry::envelope(queryGeometry);

    std::vector<std::reference_wrapper<const RenderTile>> sortedTiles{ renderTiles.begin(),
                                                                       renderTiles.end() };
    std::sort(sortedTiles.begin(), sortedTiles.end(), [](const RenderTile& a, const RenderTile& b) {
        return std::tie(a.id.canonical.z, a.id.canonical.y, a.id.wrap, a.id.canonical.x) <
            std::tie(b.id.canonical.z, b.id.canonical.y, b.id.wrap, b.id.canonical.x);
    });

    for (const RenderTile& renderTile : sortedTiles) {
        GeometryCoordinate tileSpaceBoundsMin = TileCoordinate::toGeometryCoordinate(renderTile.id, box.min);
        if (tileSpaceBoundsMin.x >= util::EXTENT || tileSpaceBoundsMin.y >= util::EXTENT) {
            continue;
        }

        GeometryCoordinate tileSpaceBoundsMax = TileCoordinate::toGeometryCoordinate(renderTile.id, box.max);
        if (tileSpaceBoundsMax.x < 0 || tileSpaceBoundsMax.y < 0) {
            continue;
        }

        GeometryCoordinates tileSpaceQueryGeometry;
        tileSpaceQueryGeometry.reserve(queryGeometry.size());
        for (const auto& c : queryGeometry) {
            tileSpaceQueryGeometry.push_back(TileCoordinate::toGeometryCoordinate(renderTile.id, c));
        }

        renderTile.tile.queryRenderedFeatures(result,
                                              tileSpaceQueryGeometry,
                                              transformState,
                                              style,
                                              options);
    }

    return result;
}

std::vector<Feature> TilePyramid::querySourceFeatures(const SourceQueryOptions& options) const {
    std::vector<Feature> result;

    for (const auto& pair : tiles) {
        pair.second->querySourceFeatures(result, options);
    }

    return result;
}

void TilePyramid::setCacheSize(size_t size) {
    cache.setSize(size);
}

void TilePyramid::onLowMemory() {
    cache.clear();
}

void TilePyramid::setObserver(TileObserver* observer_) {
    observer = observer_;
}

void TilePyramid::dumpDebugLogs() const {
    for (const auto& pair : tiles) {
        pair.second->dumpDebugLogs();
    }
}

} // namespace mbgl
