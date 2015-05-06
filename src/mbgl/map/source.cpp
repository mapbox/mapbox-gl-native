#include <mbgl/map/source.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/box.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/token.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/tile_cover.hpp>

#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/raster_tile_data.hpp>
#include <mbgl/map/live_tile_data.hpp>
#include <mbgl/style/style.hpp>

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
    std::string result = tiles.at((id.x + id.y) % tiles.size());
    result = util::mapbox::normalizeTileURL(result, url, type);
    result = util::replaceTokens(result, [&](const std::string &token) -> std::string {
        if (token == "z") return util::toString(id.z);
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

Source::~Source() {
    if (req) {
        Environment::Get().cancelRequest(req);
    }
}

bool Source::isLoaded() const {
    if (!loaded) {
        return false;
    }

    for (const auto& tile : tiles) {
        if (tile.second->data->state != TileData::State::parsed) {
            return false;
        }
    }

    return true;
}

// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Source::load(const std::string& accessToken) {
    if (info.url.empty()) {
        loaded = true;
        return;
    }

    util::ptr<Source> source = shared_from_this();

    const std::string url = util::mapbox::normalizeSourceURL(info.url, accessToken);
    req = Environment::Get().request({ Resource::Kind::JSON, url }, [source](const Response &res) {
        source->clearRequest();

        if (res.status != Response::Successful) {
            Log::Warning(Event::General, "Failed to load source TileJSON: %s", res.message.c_str());
            return;
        }

        rapidjson::Document d;
        d.Parse<0>(res.data.c_str());

        if (d.HasParseError()) {
            Log::Error(Event::General, "Invalid source TileJSON; Parse Error at %d: %s", d.GetErrorOffset(), d.GetParseError());
            return;
        }

        source->info.parseTileJSONProperties(d);
        source->loaded = true;

        source->emitSourceLoaded();
    });
}

void Source::updateMatrices(const mat4 &projMatrix, const TransformState &transform) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        transform.matrixFor(tile.matrix, tile.id);
        matrix::multiply(tile.matrix, projMatrix, tile.matrix);
    }
}

void Source::drawClippingMasks(Painter &painter) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        gl::group group(std::string { "mask: " } + std::string(tile.id));
        painter.drawClippingMask(tile.matrix, tile.clip);
    }
}

void Source::finishRender(Painter &painter) {
    for (const auto& pair : tiles) {
        Tile &tile = *pair.second;
        painter.renderTileDebug(tile);
    }
}

std::forward_list<Tile *> Source::getLoadedTiles() const {
    std::forward_list<Tile *> ptrs;
    auto it = ptrs.before_begin();
    for (const auto &pair : tiles) {
        if (pair.second->data->ready()) {
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
        Tile &tile = *it->second;
        if (tile.id == id && tile.data) {
            return tile.data->state;
        }
    }

    return TileData::State::invalid;
}

void Source::handlePartialTile(const TileID &id, Worker &worker) {
    const TileID normalized_id = id.normalized();

    auto it = tile_data.find(normalized_id);
    if (it == tile_data.end()) {
        return;
    }

    util::ptr<TileData> data = it->second.lock();
    if (!data) {
        return;
    }

    auto callback = std::bind(&Source::emitTileLoaded, this);
    data->reparse(worker, callback);
}

TileData::State Source::addTile(MapData& data,
                                const TransformState& transformState,
                                Style& style,
                                GlyphAtlas& glyphAtlas,
                                GlyphStore& glyphStore,
                                SpriteAtlas& spriteAtlas,
                                util::ptr<Sprite> sprite,
                                TexturePool& texturePool,
                                const TileID& id) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    auto pos = tiles.emplace(id, util::make_unique<Tile>(id));
    Tile& new_tile = *pos.first->second;

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const TileID normalized_id = id.normalized();

    auto it = tile_data.find(normalized_id);
    if (it != tile_data.end()) {
        // Create a shared_ptr handle. Note that this might be empty!
        new_tile.data = it->second.lock();
    }

    if (new_tile.data && new_tile.data->state == TileData::State::obsolete) {
        // Do not consider the tile if it's already obsolete.
        new_tile.data.reset();
    }

    if (!new_tile.data) {
        new_tile.data = cache.get(normalized_id.to_uint64());
    }

    auto callback = std::bind(&Source::emitTileLoaded, this);
    if (!new_tile.data) {
        // If we don't find working tile data, we're just going to load it.
        if (info.type == SourceType::Vector) {
            new_tile.data =
                std::make_shared<VectorTileData>(normalized_id, data.transform.getMaxZoom(), style, glyphAtlas,
                                                 glyphStore, spriteAtlas, sprite, info);
            new_tile.data->request(style.workers, transformState.getPixelRatio(), callback);
        } else if (info.type == SourceType::Raster) {
            new_tile.data = std::make_shared<RasterTileData>(normalized_id, texturePool, info);
            new_tile.data->request(style.workers, transformState.getPixelRatio(), callback);
        } else if (info.type == SourceType::Annotations) {
            new_tile.data = std::make_shared<LiveTileData>(normalized_id, data.annotationManager,
                                                           data.transform.getMaxZoom(), style, glyphAtlas,
                                                           glyphStore, spriteAtlas, sprite, info);
            new_tile.data->reparse(style.workers, callback);
        } else {
            throw std::runtime_error("source type not implemented");
        }
        tile_data.emplace(new_tile.data->id, new_tile.data);
    }

    return new_tile.data->state;
}

double Source::getZoom(const TransformState& state) const {
    double offset = std::log(util::tileSize / info.tile_size) / std::log(2);
    return state.getZoom() + offset;
}

int32_t Source::coveringZoomLevel(const TransformState& state) const {
    return std::floor(getZoom(state));
}

std::forward_list<TileID> Source::coveringTiles(const TransformState& state) const {
    int32_t z = coveringZoomLevel(state);

    if (z < info.min_zoom) return {{}};
    if (z > info.max_zoom) z = info.max_zoom;

    // Map four viewport corners to pixel coordinates
    box points = state.cornersToBox(z);
    const vec2<double>& center = points.center;

    std::forward_list<TileID> covering_tiles = tileCover(z, points);

    covering_tiles.sort([&center](const TileID& a, const TileID& b) {
        // Sorts by distance from the box center
        return std::fabs(a.x - center.x) + std::fabs(a.y - center.y) <
               std::fabs(b.x - center.x) + std::fabs(b.y - center.y);
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
    auto ids = id.children(z + 1);
    for (const auto& child_id : ids) {
        const TileData::State state = hasTile(child_id);
        if (TileData::isReadyState(state)) {
            retain.emplace_front(child_id);
        } else {
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
bool Source::findLoadedParent(const TileID& id, int32_t minCoveringZoom, std::forward_list<TileID>& retain) {
    for (int32_t z = id.z - 1; z >= minCoveringZoom; --z) {
        const TileID parent_id = id.parent(z);
        const TileData::State state = hasTile(parent_id);
        if (TileData::isReadyState(state)) {
            retain.emplace_front(parent_id);
            return true;
        }
    }
    return false;
}

void Source::update(MapData& data,
                    const TransformState& transformState,
                    Style& style,
                    GlyphAtlas& glyphAtlas,
                    GlyphStore& glyphStore,
                    SpriteAtlas& spriteAtlas,
                    util::ptr<Sprite> sprite,
                    TexturePool& texturePool) {
    if (!loaded || data.getAnimationTime() <= updated) {
        return;
    }

    int32_t zoom = std::floor(getZoom(transformState));
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
            handlePartialTile(id, style.workers);
            break;
        case TileData::State::invalid:
            state = addTile(data, transformState, style, glyphAtlas, glyphStore,
                            spriteAtlas, sprite, texturePool, id);
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
                                       (data.transform.getMaxZoom() - data.transform.getMinZoom() + 1) *
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
        } else if (type != SourceType::Raster && tile.data->state == TileData::State::parsed) {
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

    updated = data.getAnimationTime();
}

void Source::invalidateTiles(const std::vector<TileID>& ids) {
    cache.clear();
    for (auto& id : ids) {
        tiles.erase(id);
        tile_data.erase(id);
    }
    updateTilePtrs();
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

void Source::clearRequest() {
    req = nullptr;
}

void Source::setObserver(Observer* observer) {
    observer_ = observer;
}

void Source::emitSourceLoaded() {
    if (observer_) {
        observer_->onSourceLoaded();
    }
}

void Source::emitTileLoaded() {
    if (observer_) {
        observer_->onTileLoaded();
    }
}

}
