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

Tile* TilePyramid::getTile(const OverscaledTileID& tileID){
        auto it = tiles.find(tileID);
        return it == tiles.end() ? cache.get(tileID) : it->second.get();
}

void TilePyramid::update(const std::vector<Immutable<style::LayerProperties>>& layers,
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

    handleWrapJump(parameters.transformState.getLatLng().longitude());

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
        }

        // Only attempt prefetching in continuous mode.
        if (parameters.mode == MapMode::Continuous && type != style::SourceType::GeoJSON && type != style::SourceType::Annotations) {
            // Request lower zoom level tiles (if configured to do so) in an attempt
            // to show something on the screen faster at the cost of a little of bandwidth.
            if (parameters.prefetchZoomDelta) {
                panZoom = std::max<int32_t>(tileZoom - parameters.prefetchZoomDelta, zoomRange.min);
            }

            if (panZoom < idealZoom) {
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

    // The min and max zoom for TileRange are based on the updateRenderables algorithm.
    // Tiles are created at the ideal tile zoom or at lower zoom levels. Child
    // tiles are used from the cache, but not created.
    optional<util::TileRange> tileRange = {};
    if (bounds) {
        tileRange = util::TileRange::fromLatLngBounds(*bounds, zoomRange.min, std::min(tileZoom, (int32_t)zoomRange.max));
    }
    auto createTileFn = [&](const OverscaledTileID& tileID) -> Tile* {
        if (tileRange && !tileRange->contains(tileID.canonical)) {
            return nullptr;
        }
        std::unique_ptr<Tile> tile = cache.pop(tileID);
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
        addRenderTile(tileID, tile);
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
            addRenderTile(previouslyRenderedTile.first, tile);
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

    // Initialize render tiles fields and update the tile contained layer render data.
    for (RenderTile& renderTile : renderTiles) {
        Tile& tile = renderTile.tile;
        if (!tile.isRenderable()) continue;

        const bool holdForFade = tile.holdForFade();
        for (const auto& layerProperties : layers) {
            const auto* typeInfo = layerProperties->baseImpl->getTypeInfo();
            if (holdForFade && typeInfo->fadingTiles == LayerTypeInfo::FadingTiles::NotRequired) {
                continue;
            }
            // Update layer properties for complete tiles; for incomplete just check the presence.
            bool layerRenderableInTile = tile.isComplete() ? tile.updateLayerProperties(layerProperties)
                                                           : static_cast<bool>(tile.getBucket(*layerProperties->baseImpl));
            if (layerRenderableInTile) {
                renderTile.used = true;
                renderTile.needsClipping = (renderTile.needsClipping || typeInfo->clipping == LayerTypeInfo::Clipping::Required);
            }
        }
    }
}

void TilePyramid::handleWrapJump(float lng) {
    // On top of the regular z/x/y values, TileIDs have a `wrap` value that specify
    // which cppy of the world the tile belongs to. For example, at `lng: 10` you
    // might render z/x/y/0 while at `lng: 370` you would render z/x/y/1.
    //
    // When lng values get wrapped (going from `lng: 370` to `long: 10`) you expect
    // to see the same thing on the screen (370 degrees and 10 degrees is the same
    // place in the world) but all the TileIDs will have different wrap values.
    //
    // In order to make this transition seamless, we calculate the rounded difference of
    // "worlds" between the last frame and the current frame. If the map panned by
    // a world, then we can assign all the tiles new TileIDs with updated wrap values.
    // For example, assign z/x/y/1 a new id: z/x/y/0. It is the same tile, just rendered
    // in a different position.
    //
    // This enables us to reuse the tiles at more ideal locations and prevent flickering.

    const float lngDifference = lng - prevLng;
    const float worldDifference = lngDifference / 360;
    const int wrapDelta = ::round(worldDifference);
    prevLng = lng;

    if (wrapDelta) {
        std::map<OverscaledTileID, std::unique_ptr<Tile>> newTiles;
        for (auto& tile : tiles) {
            auto newID = tile.second->id.unwrapTo(tile.second->id.wrap + wrapDelta);
            tile.second->id = newID;
            newTiles.emplace(newID, std::move(tile.second));
        }
        tiles = std::move(newTiles);

        for (auto& renderTile : renderTiles) {
            renderTile.id = renderTile.id.unwrapTo(renderTile.id.wrap + wrapDelta);
        }
    }
}


std::unordered_map<std::string, std::vector<Feature>> TilePyramid::queryRenderedFeatures(const ScreenLineString& geometry,
                                           const TransformState& transformState,
                                           const std::vector<const RenderLayer*>& layers,
                                           const RenderedQueryOptions& options,
                                           const mat4& projMatrix) const {
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
    // TODO: Find out why we need a special sorting algorithm here.
    std::vector<std::reference_wrapper<const RenderTile>> sortedTiles{ renderTiles.begin(),
                                                                       renderTiles.end() };
    std::sort(sortedTiles.begin(), sortedTiles.end(), [](const RenderTile& a, const RenderTile& b) {
        return std::tie(a.id.canonical.z, a.id.canonical.y, a.id.wrap, a.id.canonical.x) <
            std::tie(b.id.canonical.z, b.id.canonical.y, b.id.wrap, b.id.canonical.x);
    });

    auto maxPitchScaleFactor = transformState.maxPitchScaleFactor();

    for (const RenderTile& renderTile : sortedTiles) {
        const float scale = std::pow(2, transformState.getZoom() - renderTile.id.canonical.z);
        auto queryPadding = maxPitchScaleFactor * renderTile.tile.getQueryPadding(layers) * util::EXTENT / util::tileSize / scale;

        GeometryCoordinate tileSpaceBoundsMin = TileCoordinate::toGeometryCoordinate(renderTile.id, box.min);
        if (tileSpaceBoundsMin.x - queryPadding >= util::EXTENT || tileSpaceBoundsMin.y - queryPadding >= util::EXTENT) {
            continue;
        }

        GeometryCoordinate tileSpaceBoundsMax = TileCoordinate::toGeometryCoordinate(renderTile.id, box.max);
        if (tileSpaceBoundsMax.x + queryPadding < 0 || tileSpaceBoundsMax.y + queryPadding < 0) {
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
                                              projMatrix);
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

void TilePyramid::reduceMemoryUse() {
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

void TilePyramid::clearAll() {
    tiles.clear();
    renderTiles.clear();
    cache.clear();
}

void TilePyramid::addRenderTile(const UnwrappedTileID& tileID, Tile& tile) {
    auto it = std::lower_bound(renderTiles.begin(), renderTiles.end(), tileID,
        [](const RenderTile& a, const UnwrappedTileID& id) { return a.id < id; });
    renderTiles.emplace(it, tileID, tile);
}

} // namespace mbgl
