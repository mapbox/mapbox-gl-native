#include <mbgl/renderer/tile_pyramid.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/tile_range.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/logging.hpp>

#include <mbgl/algorithm/update_renderables.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <cmath>
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

void TilePyramid::startRender(PaintParameters& parameters) {
    for (auto& tile : renderTiles) {
        tile.startRender(parameters);
    }
}

void TilePyramid::finishRender(PaintParameters& parameters) {
    for (auto& tile : renderTiles) {
        tile.finishRender(parameters);
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
                         optional<LatLngBounds> bounds,
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

        // Only attempt prefetching in continuous mode.
        if (parameters.mode == MapMode::Continuous) {
            tileZoom = idealZoom;

            // Request lower zoom level tiles (if configured to do so) in an attempt
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
    std::set<UnwrappedTileID> rendered;

    auto retainTileFn = [&](Tile& tile, TileNecessity necessity) -> void {
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
    
    optional<util::TileRange> tileRange = {};
    if (bounds) {
        tileRange = util::TileRange::fromLatLngBounds(*bounds, std::min(tileZoom, (int32_t)zoomRange.max));
    }
    auto createTileFn = [&](const OverscaledTileID& tileID) -> Tile* {
        if (tileRange && !tileRange->contains(tileID.canonical)) {
            return nullptr;
        }
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

    std::map<UnwrappedTileID, Tile*> previouslyRenderedTiles;
    for (auto& renderTile : renderTiles) {
        previouslyRenderedTiles[renderTile.id] = &renderTile.tile;
    }

    auto renderTileFn = [&](const UnwrappedTileID& tileID, Tile& tile) {
        renderTiles.emplace_back(tileID, tile);
        rendered.emplace(tileID);
        previouslyRenderedTiles.erase(tileID); // Still rendering this tile, no need for special fading logic.
        tile.markRenderedIdeal();
    };

    renderTiles.clear();

    if (!panTiles.empty()) {
        algorithm::updateRenderables(getTileFn, createTileFn, retainTileFn,
                [](const UnwrappedTileID&, Tile&) {}, panTiles, zoomRange, panZoom);
    }

    algorithm::updateRenderables(getTileFn, createTileFn, retainTileFn, renderTileFn,
                                 idealTiles, zoomRange, tileZoom);
    
    for (auto previouslyRenderedTile : previouslyRenderedTiles) {
        Tile& tile = *previouslyRenderedTile.second;
        tile.markRenderedPreviously();
        if (tile.holdForFade()) {
            // Since it was rendered in the last frame, we know we have it
            // Don't mark the tile "Required" to avoid triggering a new network request
            retainTileFn(tile, TileNecessity::Optional);
            renderTiles.emplace_back(previouslyRenderedTile.first, tile);
            rendered.emplace(previouslyRenderedTile.first);
        }
    }

    if (type != SourceType::Annotations) {
        size_t conservativeCacheSize =
            std::max((float)parameters.transformState.getSize().width / tileSize, 1.0f) *
            std::max((float)parameters.transformState.getSize().height / tileSize, 1.0f) *
            (parameters.transformState.getMaxZoom() - parameters.transformState.getMinZoom() + 1) *
            0.5;
        cache.setSize(conservativeCacheSize);
    }

    // Remove stale tiles. This goes through the (sorted!) tiles map and retain set in lockstep
    // and removes items from tiles that don't have the corresponding key in the retain set.
    {
        auto tilesIt = tiles.begin();
        auto retainIt = retain.begin();
        while (tilesIt != tiles.end()) {
            auto renderedIt = rendered.find(tilesIt->first.toUnwrapped());
            if (renderedIt == rendered.end()) {
                // Since this tile isn't in the render set, crossTileIDs won't be kept
                // updated by CrossTileSymbolIndex. We need to reset the stored crossTileIDs
                // so they're not reused if/when this tile is re-added to the render set
                tilesIt->second->resetCrossTileIDs();
            }
            if (retainIt == retain.end() || tilesIt->first < *retainIt) {
                if (!needsRelayout) {
                    tilesIt->second->setNecessity(TileNecessity::Optional);
                    cache.add(tilesIt->first, std::move(tilesIt->second));
                }
                tiles.erase(tilesIt++);
            } else {
                if (!(*retainIt < tilesIt->first)) {
                    ++tilesIt;
                }
                ++retainIt;
            }
        }
    }

    for (auto& pair : tiles) {
        pair.second->setShowCollisionBoxes(parameters.debugOptions & MapDebugOptions::Collision);
    }
}

std::unordered_map<std::string, std::vector<Feature>> TilePyramid::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const std::vector<const RenderLayer*>& layers,
                                           const RenderedQueryOptions& options,
                                           const CollisionIndex& collisionIndex) const {
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
                                              layers,
                                              options,
                                              collisionIndex);
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
