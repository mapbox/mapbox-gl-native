#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/update_parameters.hpp>
#include <mbgl/style/query_parameters.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/enum.hpp>

#include <mbgl/algorithm/update_renderables.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <algorithm>

namespace mbgl {
namespace style {

static SourceObserver nullObserver;

Source::Impl::Impl(SourceType type_, std::string id_, Source& base_)
    : type(type_),
      id(std::move(id_)),
      base(base_),
      observer(&nullObserver) {
}

Source::Impl::~Impl() = default;

bool Source::Impl::isLoaded() const {
    if (!loaded) return false;

    for (const auto& pair : tiles) {
        if (!pair.second->isComplete()) {
            return false;
        }
    }

    return true;
}

void Source::Impl::invalidateTiles() {
    tiles.clear();
    renderTiles.clear();
    cache.clear();
}

void Source::Impl::startRender(algorithm::ClipIDGenerator& generator,
                         const mat4& projMatrix,
                         const TransformState& transform) {
    if (type == SourceType::Vector ||
        type == SourceType::GeoJSON ||
        type == SourceType::Annotations) {
        generator.update(renderTiles);
    }

    for (auto& pair : renderTiles) {
        auto& tile = pair.second;
        transform.matrixFor(tile.matrix, tile.id);
        matrix::multiply(tile.matrix, projMatrix, tile.matrix);
    }
}

void Source::Impl::finishRender(Painter& painter) {
    for (auto& pair : renderTiles) {
        auto& tile = pair.second;
        painter.renderTileDebug(tile);
    }
}

const std::map<UnwrappedTileID, RenderTile>& Source::Impl::getRenderTiles() const {
    return renderTiles;
}

Tile* Source::Impl::getTile(const OverscaledTileID& overscaledTileID) const {
    auto it = tiles.find(overscaledTileID);
    if (it != tiles.end()) {
        return it->second.get();
    } else {
        return nullptr;
    }
}

bool Source::Impl::update(const UpdateParameters& parameters) {
    bool allTilesUpdated = true;

    if (!loaded || parameters.animationTime <= updated) {
        return allTilesUpdated;
    }

    const uint16_t tileSize = getTileSize();
    const Range<uint8_t> zoomRange = getZoomRange();

    // Determine the overzooming/underzooming amounts and required tiles.
    int32_t overscaledZoom = util::coveringZoomLevel(parameters.transformState.getZoom(), type, tileSize);
    int32_t dataTileZoom = overscaledZoom;

    std::vector<UnwrappedTileID> idealTiles;
    if (overscaledZoom >= zoomRange.min) {
        int32_t idealZoom = std::min<int32_t>(zoomRange.max, overscaledZoom);

        // Make sure we're not reparsing overzoomed raster tiles.
        if (type == SourceType::Raster) {
            dataTileZoom = idealZoom;
        }

        idealTiles = util::tileCover(parameters.transformState, idealZoom);
    }

    // Stores a list of all the data tiles that we're definitely going to retain. There are two
    // kinds of tiles we need: the ideal tiles determined by the tile cover. They may not yet be in
    // use because they're still loading. In addition to that, we also need to retain all tiles that
    // we're actively using, e.g. as a replacement for tile that aren't loaded yet.
    std::set<OverscaledTileID> retain;

    auto retainTileFn = [&retain](Tile& tile, bool required) -> void {
        retain.emplace(tile.id);
        tile.setNecessity(required ? Tile::Necessity::Required
                                   : Tile::Necessity::Optional);
    };
    auto getTileFn = [this](const OverscaledTileID& dataTileID) -> Tile* {
        return getTile(dataTileID);
    };
    auto createTileFn = [this, &parameters](const OverscaledTileID& dataTileID) -> Tile* {
        std::unique_ptr<Tile> data = cache.get(dataTileID);
        if (!data) {
            data = createTile(dataTileID, parameters);
            if (data) {
                data->setObserver(this);
            }
        }
        if (data) {
            return tiles.emplace(dataTileID, std::move(data)).first->second.get();
        } else {
            return nullptr;
        }
    };
    auto renderTileFn = [this](const UnwrappedTileID& renderTileID, Tile& tile) {
        renderTiles.emplace(renderTileID, RenderTile{ renderTileID, tile });
    };

    renderTiles.clear();
    algorithm::updateRenderables(getTileFn, createTileFn, retainTileFn, renderTileFn,
                                 idealTiles, zoomRange, dataTileZoom);

    if (type != SourceType::Raster && type != SourceType::Annotations && cache.getSize() == 0) {
        size_t conservativeCacheSize =
            ((float)parameters.transformState.getWidth() / util::tileSize) *
            ((float)parameters.transformState.getHeight() / util::tileSize) *
            (parameters.transformState.getMaxZoom() - parameters.transformState.getMinZoom() + 1) *
            0.5;
        cache.setSize(conservativeCacheSize);
    }

    // Remove stale data tiles from the active set of tiles.
    // This goes through the (sorted!) tiles and retain set in lockstep and removes items from
    // tiles that don't have the corresponding key in the retain set.
    auto dataIt = tiles.begin();
    auto retainIt = retain.begin();
    while (dataIt != tiles.end()) {
        if (retainIt == retain.end() || dataIt->first < *retainIt) {
            dataIt->second->setNecessity(Tile::Necessity::Optional);
            cache.add(dataIt->first, std::move(dataIt->second));
            tiles.erase(dataIt++);
        } else {
            if (!(*retainIt < dataIt->first)) {
                ++dataIt;
            }
            ++retainIt;
        }
    }

    const PlacementConfig newConfig{ parameters.transformState.getAngle(),
                                     parameters.transformState.getPitch(),
                                     parameters.debugOptions & MapDebugOptions::Collision };
    for (auto& pair : tiles) {
        auto tile = pair.second.get();
        if (parameters.shouldReparsePartialTiles && tile->isIncomplete()) {
            if (!tile->parsePending()) {
                allTilesUpdated = false;
            }
        } else {
            tile->redoPlacement(newConfig);
        }
    }

    updated = parameters.animationTime;

    return allTilesUpdated;
}

static Point<int16_t> coordinateToTilePoint(const UnwrappedTileID& tileID, const Point<double>& p) {
    auto zoomedCoord = TileCoordinate { p, 0 }.zoomTo(tileID.canonical.z);
    return {
        int16_t(util::clamp<int64_t>((zoomedCoord.p.x - tileID.canonical.x - tileID.wrap * std::pow(2, tileID.canonical.z)) * util::EXTENT,
                    std::numeric_limits<int16_t>::min(),
                    std::numeric_limits<int16_t>::max())),
        int16_t(util::clamp<int64_t>((zoomedCoord.p.y - tileID.canonical.y) * util::EXTENT,
                    std::numeric_limits<int16_t>::min(),
                    std::numeric_limits<int16_t>::max()))
    };
}

std::unordered_map<std::string, std::vector<Feature>> Source::Impl::queryRenderedFeatures(const QueryParameters& parameters) const {
    LineString<double> queryGeometry;

    for (const auto& p : parameters.geometry) {
        queryGeometry.push_back(TileCoordinate::fromScreenCoordinate(
            parameters.transformState, 0, { p.x, parameters.transformState.getHeight() - p.y }).p);
    }

    mapbox::geometry::box<double> box = mapbox::geometry::envelope(queryGeometry);

    std::unordered_map<std::string, std::vector<Feature>> result;

    for (const auto& tilePtr : renderTiles) {
        const RenderTile& tile = tilePtr.second;

        Point<int16_t> tileSpaceBoundsMin = coordinateToTilePoint(tile.id, box.min);
        Point<int16_t> tileSpaceBoundsMax = coordinateToTilePoint(tile.id, box.max);

        if (tileSpaceBoundsMin.x >= util::EXTENT || tileSpaceBoundsMin.y >= util::EXTENT ||
            tileSpaceBoundsMax.x < 0 || tileSpaceBoundsMax.y < 0) continue;

        GeometryCoordinates tileSpaceQueryGeometry;

        for (const auto& c : queryGeometry) {
            tileSpaceQueryGeometry.push_back(coordinateToTilePoint(tile.id, c));
        }

        tile.tile.queryRenderedFeatures(result,
                                        tileSpaceQueryGeometry,
                                        parameters.transformState,
                                        parameters.layerIDs);
    }

    return result;
}

void Source::Impl::setCacheSize(size_t size) {
    cache.setSize(size);
}

void Source::Impl::onLowMemory() {
    cache.clear();
}

void Source::Impl::setObserver(SourceObserver* observer_) {
    observer = observer_;
}

void Source::Impl::onTileLoaded(Tile& tile, bool isNewTile) {
    observer->onTileLoaded(base, tile.id, isNewTile);
}

void Source::Impl::onTileError(Tile& tile, std::exception_ptr error) {
    observer->onTileError(base, tile.id, error);
}

void Source::Impl::onNeedsRepaint() {
    observer->onNeedsRepaint();
}

void Source::Impl::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", base.getID().c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);

    for (const auto& pair : tiles) {
        auto& tile = pair.second;
        tile->dumpDebugLogs();
    }
}

} // namespace style
} // namespace mbgl
