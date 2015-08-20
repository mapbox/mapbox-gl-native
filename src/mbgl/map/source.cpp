#include <mbgl/map/source.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
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
#include <mbgl/platform/log.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/tile_cover.hpp>

#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/gl/debugging.hpp>

#include <rapidjson/error/en.h>

#include <algorithm>

namespace mbgl {

void parse(const rapidjson::Value& value, std::vector<std::string>& target, const char *name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsArray())
        return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        if (!property[i].IsString())
            return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        target.emplace_back(std::string(property[i].GetString(), property[i].GetStringLength()));
}

void parse(const rapidjson::Value& value, std::string& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsString())
        return;

    target = { property.GetString(), property.GetStringLength() };
}

void parse(const rapidjson::Value& value, uint16_t& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsUint())
        return;

    unsigned int uint = property.GetUint();
    if (uint > std::numeric_limits<uint16_t>::max())
        return;

    target = uint;
}

template <size_t N>
void parse(const rapidjson::Value& value, std::array<float, N>& target, const char* name) {
    if (!value.HasMember(name))
        return;

    const rapidjson::Value& property = value[name];
    if (!property.IsArray() || property.Size() != N)
        return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        if (!property[i].IsNumber())
            return;

    for (rapidjson::SizeType i = 0; i < property.Size(); i++)
        target[i] = property[i].GetDouble();
}

void SourceInfo::parseTileJSONProperties(const rapidjson::Value& value) {
    parse(value, tiles, "tiles");
    parse(value, min_zoom, "minzoom");
    parse(value, max_zoom, "maxzoom");
    parse(value, attribution, "attribution");
    parse(value, center, "center");
    parse(value, bounds, "bounds");
}

std::string SourceInfo::tileURL(const TileID& id, float pixelRatio) const {
    std::string result = tiles.at(0);
    result = util::mapbox::normalizeTileURL(result, url, type);
    result = util::replaceTokens(result, [&](const std::string &token) -> std::string {
        if (token == "z") return util::toString(std::min(id.z, static_cast<int8_t>(max_zoom)));
        if (token == "x") return util::toString(id.x);
        if (token == "y") return util::toString(id.y);
        if (token == "prefix") {
            std::string prefix { 2 };
            prefix[0] = "0123456789abcdef"[id.x % 16];
            prefix[1] = "0123456789abcdef"[id.y % 16];
            return prefix;
        }
        if (token == "ratio") return pixelRatio > 1.0 ? "@2x" : "";
        return "";
    });
    return result;
}

Source::Source() {}

Source::~Source() = default;

bool Source::isLoaded() const {
    if (!loaded) {
        return false;
    }

    for (const auto& tile : tiles) {
        if (tile.second->data->getState() != TileData::State::parsed) {
            return false;
        }
    }

    return true;
}

// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Source::load() {
    if (info.url.empty()) {
        loaded = true;
        return;
    }

    FileSource* fs = util::ThreadContext::getFileSource();
    req = fs->request({ Resource::Kind::Source, info.url }, util::RunLoop::getLoop(), [this](const Response &res) {
        if (res.stale) {
            // Only handle fresh responses.
            return;
        }
        req = nullptr;

        if (res.error) {
            std::stringstream message;
            message <<  "Failed to load [" << info.url << "]: " << res.error->message;
            emitSourceLoadingFailed(message.str());
            return;
        }

        rapidjson::Document d;
        d.Parse<0>(res.data->c_str());

        if (d.HasParseError()) {
            std::stringstream message;
            message << "Failed to parse [" << info.url << "]: " << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
            emitSourceLoadingFailed(message.str());
            return;
        }

        info.parseTileJSONProperties(d);
        loaded = true;

        emitSourceLoaded();
    });
}

void Source::updateMatrices(const mat4 &projMatrix, const TransformState &transform) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        transform.matrixFor(tile.matrix, tile.id, std::min(static_cast<int8_t>(info.max_zoom), tile.id.z));
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

TileData::State Source::hasTile(const TileID& id) {
    auto it = tiles.find(id);
    if (it != tiles.end()) {
        Tile& tile = *it->second;
        if (tile.id == id && tile.data) {
            return tile.data->getState();
        }
    }

    return TileData::State::invalid;
}

bool Source::handlePartialTile(const TileID& id, Worker&) {
    const TileID normalized_id = id.normalized();

    auto it = tile_data.find(normalized_id);
    if (it == tile_data.end()) {
        return true;
    }

    auto data = it->second.lock();
    if (!data) {
        return true;
    }

    return data->parsePending([this]() {
        emitTileLoaded(false);
    });
}

TileData::State Source::addTile(MapData& data,
                                const TransformState& transformState,
                                Style& style,
                                TexturePool& texturePool,
                                const TileID& id) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    auto pos = tiles.emplace(id, std::make_unique<Tile>(id));

    Tile& new_tile = *pos.first->second;

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const TileID normalized_id = id.normalized();

    auto it = tile_data.find(normalized_id);
    if (it != tile_data.end()) {
        // Create a shared_ptr handle. Note that this might be empty!
        new_tile.data = it->second.lock();
    }

    if (new_tile.data && new_tile.data->getState() == TileData::State::obsolete) {
        // Do not consider the tile if it's already obsolete.
        new_tile.data.reset();
    }

    if (!new_tile.data) {
        new_tile.data = cache.get(normalized_id.to_uint64());
    }

    if (!new_tile.data) {
        auto callback = std::bind(&Source::tileLoadingCompleteCallback, this, normalized_id, transformState, data.getCollisionDebug());

        // If we don't find working tile data, we're just going to load it.
        if (info.type == SourceType::Raster) {
            auto tileData = std::make_shared<RasterTileData>(normalized_id, texturePool, info, style.workers);
            tileData->request(data.pixelRatio, callback);
            new_tile.data = tileData;
        } else {
            std::unique_ptr<GeometryTileMonitor> monitor;

            if (info.type == SourceType::Vector) {
                monitor = std::make_unique<VectorTileMonitor>(info, normalized_id, data.pixelRatio);
            } else if (info.type == SourceType::Annotations) {
                monitor = std::make_unique<AnnotationTileMonitor>(normalized_id, data);
            } else {
                throw std::runtime_error("source type not implemented");
            }

            new_tile.data = std::make_shared<VectorTileData>(normalized_id,
                                                             std::move(monitor),
                                                             info.source_id,
                                                             style,
                                                             callback);
        }

        tile_data.emplace(new_tile.data->id, new_tile.data);
    }

    return new_tile.data->getState();
}

double Source::getZoom(const TransformState& state) const {
    double offset = std::log(util::tileSize / info.tile_size) / std::log(2);
    return state.getZoom() + offset;
}

int32_t Source::coveringZoomLevel(const TransformState& state) const {
    double zoom = getZoom(state);
    if (info.type == SourceType::Raster || info.type == SourceType::Video) {
        zoom = ::round(zoom);
    } else {
        zoom = std::floor(zoom);
    }
    return zoom;
}

std::forward_list<TileID> Source::coveringTiles(const TransformState& state) const {
    int32_t z = coveringZoomLevel(state);

    auto actualZ = z;
    const bool reparseOverscaled =
        info.type == SourceType::Vector ||
        info.type == SourceType::Annotations;

    if (z < info.min_zoom) return {{}};
    if (z > info.max_zoom) z = info.max_zoom;

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
bool Source::findLoadedChildren(const TileID& id, int32_t maxCoveringZoom, std::forward_list<TileID>& retain) {
    bool complete = true;
    int32_t z = id.z;
    auto ids = id.children(info.max_zoom);
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
void Source::findLoadedParent(const TileID& id, int32_t minCoveringZoom, std::forward_list<TileID>& retain) {
    for (int32_t z = id.z - 1; z >= minCoveringZoom; --z) {
        const TileID parent_id = id.parent(z, info.max_zoom);
        const TileData::State state = hasTile(parent_id);
        if (TileData::isReadyState(state)) {
            retain.emplace_front(parent_id);
            if (state == TileData::State::parsed) {
                return;
            }
        }
    }
}

bool Source::update(MapData& data,
                    const TransformState& transformState,
                    Style& style,
                    TexturePool& texturePool,
                    bool shouldReparsePartialTiles) {
    bool allTilesUpdated = true;

    if (!loaded || data.getAnimationTime() <= updated) {
        return allTilesUpdated;
    }

    double zoom = getZoom(transformState);
    if (info.type == SourceType::Raster || info.type == SourceType::Video) {
        zoom = ::round(zoom);
    } else {
        zoom = std::floor(zoom);
    }
    std::forward_list<TileID> required = coveringTiles(transformState);

    // Determine the overzooming/underzooming amounts.
    int32_t minCoveringZoom = util::clamp<int32_t>(zoom - 10, info.min_zoom, info.max_zoom);
    int32_t maxCoveringZoom = util::clamp<int32_t>(zoom + 1,  info.min_zoom, info.max_zoom);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<TileID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const auto& id : required) {
        TileData::State state = hasTile(id);

        switch (state) {
        case TileData::State::partial:
            if (shouldReparsePartialTiles) {
                if (!handlePartialTile(id, style.workers)) {
                    allTilesUpdated = false;
                }
            }
            break;
        case TileData::State::invalid:
            state = addTile(data, transformState, style, texturePool, id);
            break;
        default:
            break;
        }

        if (!TileData::isReadyState(state)) {
            // The tile we require is not yet loaded. Try to find a parent or
            // child tile that we already have.

            // First, try to find existing child tiles that completely cover the
            // missing tile.
            bool complete = findLoadedChildren(id, maxCoveringZoom, retain);

            // Then, if there are no complete child tiles, try to find existing
            // parent tiles that completely cover the missing tile.
            if (!complete) {
                findLoadedParent(id, minCoveringZoom, retain);
            }
        }
    }

    if (info.type != SourceType::Raster && cache.getSize() == 0) {
        size_t conservativeCacheSize = ((float)transformState.getWidth()  / util::tileSize) *
                                       ((float)transformState.getHeight() / util::tileSize) *
                                       (transformState.getMaxZoom() - transformState.getMinZoom() + 1) *
                                       0.5;
        cache.setSize(conservativeCacheSize);
    }

    auto& tileCache = cache;
    auto& type = info.type;

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    std::set<TileID> retain_data;
    util::erase_if(tiles, [&retain, &retain_data, &tileCache, &type](std::pair<const TileID, std::unique_ptr<Tile>> &pair) {
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
    util::erase_if(tile_data, [&retain_data, &tileCache](std::pair<const TileID, std::weak_ptr<TileData>> &pair) {
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
            { transformState.getAngle(), transformState.getPitch(), data.getCollisionDebug() });
    }

    updated = data.getAnimationTime();

    return allTilesUpdated;
}

void Source::updateTilePtrs() {
    std::vector<Tile*> newPtrs;
    for (const auto& pair : tiles) {
        newPtrs.push_back(pair.second.get());
    }

    if (tilePtrs != newPtrs) {
        tilePtrs.swap(newPtrs);
        emitTileLoaded(true);
    }
}

void Source::setCacheSize(size_t size) {
    cache.setSize(size);
}

void Source::onLowMemory() {
    cache.clear();
}

void Source::setObserver(Observer* observer) {
    observer_ = observer;
}

void Source::tileLoadingCompleteCallback(const TileID& normalized_id, const TransformState& transformState, bool collisionDebug) {
    auto it = tile_data.find(normalized_id);
    if (it == tile_data.end()) {
        return;
    }

    util::ptr<TileData> data = it->second.lock();
    if (!data) {
        return;
    }

    if (data->getState() == TileData::State::obsolete && !data->getError().empty()) {
        emitTileLoadingFailed(data->getError());
        return;
    }

    data->redoPlacement({ transformState.getAngle(), transformState.getPitch(), collisionDebug });
    emitTileLoaded(true);
}

void Source::emitSourceLoaded() {
    if (observer_) {
        observer_->onSourceLoaded();
    }
}

void Source::emitSourceLoadingFailed(const std::string& message) {
    if (!observer_) {
        return;
    }

    auto error = std::make_exception_ptr(util::SourceLoadingException(message));
    observer_->onSourceLoadingFailed(error);
}

void Source::emitTileLoaded(bool isNewTile) {
    if (observer_) {
        observer_->onTileLoaded(isNewTile);
    }
}

void Source::emitTileLoadingFailed(const std::string& message) {
    if (!observer_) {
        return;
    }

    auto error = std::make_exception_ptr(util::TileLoadingException(message));
    observer_->onTileLoadingFailed(error);
}

void Source::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", info.source_id.c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);

    for (const auto& tile : tiles) {
        tile.second->data->dumpDebugLogs();
    }
}

}
