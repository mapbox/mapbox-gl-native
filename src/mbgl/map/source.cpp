#include <mbgl/map/source.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/tile/geojson_tile.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/box.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_update_parameters.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/url.hpp>

#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_parser.hpp>
#include <mbgl/gl/debugging.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/convert.hpp>

#include <rapidjson/error/en.h>

#include <algorithm>
#include <sstream>

namespace mbgl {

Source::Source(SourceType type_,
               const std::string& id_,
               const std::string& url_,
               uint16_t tileSize_,
               std::unique_ptr<SourceInfo>&& info_,
               std::unique_ptr<mapbox::geojsonvt::GeoJSONVT>&& geojsonvt_)
    : type(type_),
      id(id_),
      url(url_),
      tileSize(tileSize_),
      info(std::move(info_)),
      geojsonvt(std::move(geojsonvt_)) {
}

Source::~Source() = default;

bool Source::isLoaded() const {
    if (!loaded) return false;

    for (const auto& tile : tiles) {
        if (tile.second->data->getState() != TileData::State::parsed) {
            return false;
        }
    }

    return true;
}

bool Source::isLoading() const {
    return !loaded && req.operator bool();
}

// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Source::load() {
    if (url.empty()) {
        // In case there is no URL set, we assume that we already have all of the data because the
        // TileJSON was specified inline in the stylesheet.
        loaded = true;
        return;
    }

    if (req) {
        // We don't have a SourceInfo object yet, but there's already a request underway to load
        // the data.
        return;
    }

    // URL may either be a TileJSON file, or a GeoJSON file.
    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Source, url }, [this](Response res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
            return;
        }

        if (res.notModified) {
            // We got the same data back as last time. Abort early.
            return;
        }

        rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
        d.Parse<0>(res.data->c_str());

        if (d.HasParseError()) {
            std::stringstream message;
            message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(message.str())));
            return;
        }

        bool reloadTiles = false;
        if (type == SourceType::Vector || type == SourceType::Raster) {
            // Create a new copy of the SourceInfo object that holds the base values we've parsed
            // from the stylesheet. Then merge in the values parsed from the TileJSON we retrieved
            // via the URL.
            auto newInfo = StyleParser::parseTileJSON(d);

            // TODO: Remove this hack by delivering proper URLs in the TileJSON to begin with.
            if (type == SourceType::Raster && util::mapbox::isMapboxURL(url)) {
                // We need to insert {ratio} into raster source URLs that are loaded from mapbox://
                // TileJSONs.
                std::transform(newInfo->tiles.begin(), newInfo->tiles.end(), newInfo->tiles.begin(),
                               util::mapbox::normalizeRasterTileURL);
            }

            // Check whether previous information specifies different tile
            if (info && info->tiles != newInfo->tiles) {
                reloadTiles = true;

                // Tile size changed: We need to recalculate the tiles we need to load because we
                // might have to load tiles for a different zoom level
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Min/Max zoom changed: We need to recalculate what tiles to load, if we have tiles
                // loaded that are outside the new zoom range
                // This is done automatically when we trigger the onSourceLoaded observer below.

                // Attribution changed: We need to notify the embedding application that this
                // changed. See https://github.com/mapbox/mapbox-gl-native/issues/2723
                // This is not yet implemented.

                // Center/bounds changed: We're not using these values currently
            }

            info = std::move(newInfo);
        } else if (type == SourceType::GeoJSON) {
            info = std::make_unique<SourceInfo>();
            geojsonvt = StyleParser::parseGeoJSON(d);
            reloadTiles = true;
        }

        if (reloadTiles) {
            // Tile information changed because we got new GeoJSON data, or a new tile URL.
            tilePtrs.clear();
            tileDataMap.clear();
            tiles.clear();
            cache.clear();
        }

        loaded = true;
        observer->onSourceLoaded(*this);
    });
}

void Source::updateMatrices(const mat4 &projMatrix, const TransformState &transform) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        transform.matrixFor(tile.matrix, tile.id, std::min(static_cast<int8_t>(info->max_zoom), tile.id.z));
        matrix::multiply(tile.matrix, projMatrix, tile.matrix);
    }
}

void Source::drawClippingMasks(Painter &painter) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        MBGL_DEBUG_GROUP(std::string { "mask: " } + std::string(tile.id));
        painter.drawClippingMask(tile.matrix, tile.clip);
    }
}

void Source::finishRender(Painter &painter) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        painter.renderTileDebug(tile);
    }
}

std::forward_list<Tile*> Source::getLoadedTiles() const {
    std::forward_list<Tile*> ptrs;
    auto it = ptrs.before_begin();
    for (const auto& pair : tiles) {
        if (pair.second->data->isReady()) {
            it = ptrs.insert_after(it, pair.second.get());
        }
    }
    return ptrs;
}

const std::vector<Tile*>& Source::getTiles() const {
    return tilePtrs;
}

TileData::State Source::hasTile(const TileID& tileID) {
    auto it = tiles.find(tileID);
    if (it != tiles.end()) {
        Tile& tile = *it->second;
        if (tile.id == tileID && tile.data) {
            return tile.data->getState();
        }
    }

    return TileData::State::invalid;
}

bool Source::handlePartialTile(const TileID& tileID, Worker&) {
    auto it = tileDataMap.find(tileID.normalized());
    if (it == tileDataMap.end()) {
        return true;
    }

    auto tileData = it->second.lock();
    if (!tileData) {
        return true;
    }

    return tileData->parsePending([this, tileID](std::exception_ptr error) {
        if (error) {
            observer->onTileError(*this, tileID, error);
            return;
        }

        observer->onTileLoaded(*this, tileID, false);
    });
}

TileData::State Source::addTile(const TileID& tileID, const StyleUpdateParameters& parameters) {
    const TileData::State state = hasTile(tileID);

    if (state != TileData::State::invalid) {
        return state;
    }

    auto newTile = std::make_unique<Tile>(tileID);

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const TileID normalizedID = tileID.normalized();

    auto it = tileDataMap.find(normalizedID);
    if (it != tileDataMap.end()) {
        // Create a shared_ptr handle. Note that this might be empty!
        newTile->data = it->second.lock();
    }

    if (newTile->data && newTile->data->getState() == TileData::State::obsolete) {
        // Do not consider the tile if it's already obsolete.
        newTile->data.reset();
    }

    if (!newTile->data) {
        newTile->data = cache.get(normalizedID.to_uint64());
    }

    if (!newTile->data) {
        auto callback = std::bind(&Source::tileLoadingCompleteCallback, this, normalizedID,
                                  std::placeholders::_1, parameters.transformState,
                                  parameters.debugOptions & MapDebugOptions::Collision);

        // If we don't find working tile data, we're just going to load it.
        if (type == SourceType::Raster) {
            auto tileData = std::make_shared<RasterTileData>(normalizedID,
                                                             parameters.texturePool,
                                                             parameters.worker);
            tileData->request(util::templateTileURL(info->tiles.at(0), normalizedID, parameters.pixelRatio), callback);
            newTile->data = tileData;
        } else {
            std::unique_ptr<GeometryTileMonitor> monitor;

            if (type == SourceType::Vector) {
                monitor = std::make_unique<VectorTileMonitor>(normalizedID, info->tiles.at(0));
            } else if (type == SourceType::Annotations) {
                monitor = std::make_unique<AnnotationTileMonitor>(normalizedID, parameters.data);
            } else if (type == SourceType::GeoJSON) {
                monitor = std::make_unique<GeoJSONTileMonitor>(geojsonvt.get(), normalizedID);
            } else {
                Log::Warning(Event::Style, "Source type '%s' is not implemented", SourceTypeClass(type).c_str());
                return TileData::State::invalid;
            }

            newTile->data = std::make_shared<VectorTileData>(normalizedID,
                                                             std::move(monitor),
                                                             id,
                                                             parameters.style,
                                                             parameters.mode,
                                                             callback);
        }

        tileDataMap.emplace(newTile->data->id, newTile->data);
    }

    const auto newState = newTile->data->getState();
    tiles.emplace(tileID, std::move(newTile));
    return newState;
}

double Source::getZoom(const TransformState& state) const {
    double offset = std::log(util::tileSize / tileSize) / std::log(2);
    return state.getZoom() + offset;
}

int32_t Source::coveringZoomLevel(const TransformState& state) const {
    double zoom = getZoom(state);
    if (type == SourceType::Raster || type == SourceType::Video) {
        zoom = ::round(zoom);
    } else {
        zoom = std::floor(zoom);
    }
    return util::clamp(zoom, state.getMinZoom(), state.getMaxZoom());
}

std::forward_list<TileID> Source::coveringTiles(const TransformState& state) const {
    int32_t z = coveringZoomLevel(state);

    auto actualZ = z;
    const bool reparseOverscaled =
        type == SourceType::Vector ||
        type == SourceType::Annotations;

    if (z < info->min_zoom) return {{}};
    if (z > info->max_zoom) z = info->max_zoom;

    // Map four viewport corners to pixel coordinates
    box points = state.cornersToBox(z);
    const TileCoordinate center = state.pointToCoordinate({ state.getWidth() / 2.0f, state.getHeight()/ 2.0f }).zoomTo(z);

    std::forward_list<TileID> covering_tiles = tileCover(z, points, reparseOverscaled ? actualZ : z);

    covering_tiles.sort([&center](const TileID& a, const TileID& b) {
        // Sorts by distance from the box center
        return std::fabs(a.x - center.column) + std::fabs(a.y - center.row) <
               std::fabs(b.x - center.column) + std::fabs(b.y - center.row);
    });

    return covering_tiles;
}

/**
 * Recursively find children of the given tile that are already loaded.
 *
 * @param id The tile ID that we should find children for.
 * @param maxCoveringZoom The maximum zoom level of children to look for.
 * @param retain An object that we add the found tiles to.
 *
 * @return boolean Whether the children found completely cover the tile.
 */
bool Source::findLoadedChildren(const TileID& tileID, int32_t maxCoveringZoom, std::forward_list<TileID>& retain) {
    bool complete = true;
    int32_t z = tileID.z;
    auto ids = tileID.children(info->max_zoom);
    for (const auto& child_id : ids) {
        const TileData::State state = hasTile(child_id);
        if (TileData::isReadyState(state)) {
            retain.emplace_front(child_id);
        }
        if (state != TileData::State::parsed) {
            complete = false;
            if (z < maxCoveringZoom) {
                // Go further down the hierarchy to find more unloaded children.
                findLoadedChildren(child_id, maxCoveringZoom, retain);
            }
        }
    }
    return complete;
}

/**
 * Find a loaded parent of the given tile.
 *
 * @param id The tile ID that we should find children for.
 * @param minCoveringZoom The minimum zoom level of parents to look for.
 * @param retain An object that we add the found tiles to.
 *
 * @return boolean Whether a parent was found.
 */
void Source::findLoadedParent(const TileID& tileID, int32_t minCoveringZoom, std::forward_list<TileID>& retain) {
    for (int32_t z = tileID.z - 1; z >= minCoveringZoom; --z) {
        const TileID parent_id = tileID.parent(z, info->max_zoom);
        const TileData::State state = hasTile(parent_id);
        if (TileData::isReadyState(state)) {
            retain.emplace_front(parent_id);
            if (state == TileData::State::parsed) {
                return;
            }
        }
    }
}

bool Source::update(const StyleUpdateParameters& parameters) {
    bool allTilesUpdated = true;

    if (!loaded || parameters.animationTime <= updated) {
        return allTilesUpdated;
    }

    double zoom = coveringZoomLevel(parameters.transformState);
    std::forward_list<TileID> required = coveringTiles(parameters.transformState);

    // Determine the overzooming/underzooming amounts.
    int32_t minCoveringZoom = util::clamp<int32_t>(zoom - 10, info->min_zoom, info->max_zoom);
    int32_t maxCoveringZoom = util::clamp<int32_t>(zoom + 1,  info->min_zoom, info->max_zoom);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<TileID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const auto& tileID : required) {
        TileData::State state = hasTile(tileID);

        switch (state) {
        case TileData::State::partial:
            if (parameters.shouldReparsePartialTiles) {
                if (!handlePartialTile(tileID, parameters.worker)) {
                    allTilesUpdated = false;
                }
            }
            break;
        case TileData::State::invalid:
            state = addTile(tileID, parameters);
            break;
        default:
            break;
        }

        if (!TileData::isReadyState(state)) {
            // The tile we require is not yet loaded. Try to find a parent or
            // child tile that we already have.

            // First, try to find existing child tiles that completely cover the
            // missing tile.
            bool complete = findLoadedChildren(tileID, maxCoveringZoom, retain);

            // Then, if there are no complete child tiles, try to find existing
            // parent tiles that completely cover the missing tile.
            if (!complete) {
                findLoadedParent(tileID, minCoveringZoom, retain);
            }
        }
    }

    if (type != SourceType::Raster && cache.getSize() == 0) {
        size_t conservativeCacheSize = ((float)parameters.transformState.getWidth()  / util::tileSize) *
                                       ((float)parameters.transformState.getHeight() / util::tileSize) *
                                       (parameters.transformState.getMaxZoom() - parameters.transformState.getMinZoom() + 1) *
                                       0.5;
        cache.setSize(conservativeCacheSize);
    }

    auto& tileCache = cache;

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    std::set<TileID> retain_data;
    util::erase_if(tiles, [this, &retain, &retain_data, &tileCache](std::pair<const TileID, std::unique_ptr<Tile>> &pair) {
        Tile &tile = *pair.second;
        bool obsolete = std::find(retain.begin(), retain.end(), tile.id) == retain.end();
        if (!obsolete) {
            retain_data.insert(tile.data->id);
        } else if (type != SourceType::Raster && tile.data->getState() == TileData::State::parsed) {
            // Partially parsed tiles are never added to the cache because otherwise
            // they never get updated if the go out from the viewport and the pending
            // resources arrive.
            tileCache.add(tile.id.normalized().to_uint64(), tile.data);
        }
        return obsolete;
    });

    // Remove all the expired pointers from the set.
    util::erase_if(tileDataMap, [&retain_data, &tileCache](std::pair<const TileID, std::weak_ptr<TileData>> &pair) {
        const util::ptr<TileData> tile = pair.second.lock();
        if (!tile) {
            return true;
        }

        bool obsolete = retain_data.find(tile->id) == retain_data.end();
        if (obsolete) {
            if (!tileCache.has(tile->id.normalized().to_uint64())) {
                tile->cancel();
            }
            return true;
        } else {
            return false;
        }
    });

    updateTilePtrs();

    for (auto& tilePtr : tilePtrs) {
        tilePtr->data->redoPlacement(
            { parameters.transformState.getAngle(), parameters.transformState.getPitch(), parameters.debugOptions & MapDebugOptions::Collision });
    }

    updated = parameters.animationTime;

    return allTilesUpdated;
}

void Source::updateTilePtrs() {
    tilePtrs.clear();
    for (const auto& pair : tiles) {
        tilePtrs.push_back(pair.second.get());
    }
}

void Source::setCacheSize(size_t size) {
    cache.setSize(size);
}

void Source::onLowMemory() {
    cache.clear();
}

void Source::setObserver(Observer* observer_) {
    observer = observer_;
}

void Source::tileLoadingCompleteCallback(const TileID& tileID,
                                         std::exception_ptr error,
                                         const TransformState& transformState,
                                         bool collisionDebug) {
    auto it = tileDataMap.find(tileID);
    if (it == tileDataMap.end()) {
        return;
    }

    util::ptr<TileData> tileData = it->second.lock();
    if (!tileData) {
        return;
    }

    if (error) {
        observer->onTileError(*this, tileID, error);
        return;
    }

    tileData->redoPlacement({ transformState.getAngle(), transformState.getPitch(), collisionDebug });
    observer->onTileLoaded(*this, tileID, true);
}

void Source::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", id.c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);

    for (const auto& tile : tiles) {
        tile.second->data->dumpDebugLogs();
    }
}

} // namespace mbgl
