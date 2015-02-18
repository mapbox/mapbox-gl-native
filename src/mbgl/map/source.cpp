#include <mbgl/map/source.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/box.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/raster_tile_data.hpp>

#include <algorithm>

namespace mbgl {

Source::Source(SourceInfo& info_)
    : info(info_)
{
}

// Note: This is a separate function that must be called exactly once after creation
// The reason this isn't part of the constructor is that calling shared_from_this() in
// the constructor fails.
void Source::load(Map& map, FileSource& fileSource) {
    if (info.url.empty()) {
        loaded = true;
        return;
    }

    util::ptr<Source> source = shared_from_this();

    const std::string url = util::mapbox::normalizeSourceURL(info.url, map.getAccessToken());
    fileSource.request({ Resource::Kind::JSON, url }, **map.loop, [source, &map](const Response &res) {
        if (res.status != Response::Successful) {
            Log::Warning(Event::General, "Failed to load source TileJSON: %s", res.message.c_str());
            return;
        }

        rapidjson::Document d;
        d.Parse<0>(res.data.c_str());

        if (d.HasParseError()) {
            Log::Warning(Event::General, "Invalid source TileJSON; Parse Error at %d: %s", d.GetErrorOffset(), d.GetParseError());
            return;
        }

        source->info.parseTileJSONProperties(d);
        source->loaded = true;

        map.update();

    });
}

void Source::updateClipIDs(const std::map<Tile::ID, ClipID> &mapping) {
    std::for_each(tiles.begin(), tiles.end(), [&mapping](std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair) {
        Tile &tile = *pair.second;
        auto it = mapping.find(tile.id);
        if (it != mapping.end()) {
            tile.clip = it->second;
        } else {
            tile.clip = ClipID {};
        }
    });
}

void Source::updateMatrices(const mat4 &projMatrix, const TransformState &transform) {
    for (std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair : tiles) {
        Tile &tile = *pair.second;
        transform.matrixFor(tile.matrix, tile.id);
        matrix::multiply(tile.matrix, projMatrix, tile.matrix);
    }
}

size_t Source::getTileCount() const {
    return tiles.size();
}

void Source::drawClippingMasks(Painter &painter) {
    for (std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair : tiles) {
        Tile &tile = *pair.second;
        gl::group group(std::string { "mask: " } + std::string(tile.id));
        painter.drawClippingMask(tile.matrix, tile.clip);
    }
}

void Source::render(Painter &painter, util::ptr<StyleLayer> layer_desc) {
    gl::group group(std::string { "layer: " } + layer_desc->id);
    for (const std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair : tiles) {
        Tile &tile = *pair.second;
        if (tile.data && tile.data->state == TileData::State::parsed) {
            painter.renderTileLayer(tile, layer_desc, tile.matrix);
        }
    }
}

void Source::render(Painter &painter, util::ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix) {
    auto it = tiles.find(id);
    if (it != tiles.end() && it->second->data && it->second->data->state == TileData::State::parsed) {
        painter.renderTileLayer(*it->second, layer_desc, matrix);
    }
}

void Source::finishRender(Painter &painter) {
    for (std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair : tiles) {
        Tile &tile = *pair.second;
        painter.renderTileDebug(tile);
    }
}

std::forward_list<Tile::ID> Source::getIDs() const {
    std::forward_list<Tile::ID> ptrs;

    std::transform(tiles.begin(), tiles.end(), std::front_inserter(ptrs), [](const std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair) {
        Tile &tile = *pair.second;
        return tile.id;
    });
    return ptrs;
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


TileData::State Source::hasTile(const Tile::ID& id) {
    auto it = tiles.find(id);
    if (it != tiles.end()) {
        Tile &tile = *it->second;
        if (tile.id == id && tile.data) {
            return tile.data->state;
        }
    }

    return TileData::State::invalid;
}

TileData::State Source::addTile(Map& map, uv::worker& worker,
                                util::ptr<Style> style,
                                GlyphAtlas& glyphAtlas, GlyphStore& glyphStore,
                                SpriteAtlas& spriteAtlas, util::ptr<Sprite> sprite,
                                FileSource& fileSource, uv_loop_t &loop, TexturePool& texturePool,
                                const Tile::ID& id,
                                std::function<void ()> callback) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    auto pos = tiles.emplace(id, util::make_unique<Tile>(id));
    Tile& new_tile = *pos.first->second;

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const Tile::ID normalized_id = id.normalized();

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
        // If we don't find working tile data, we're just going to load it.
        if (info.type == SourceType::Vector) {
            new_tile.data = std::make_shared<VectorTileData>(normalized_id, map.getMaxZoom(), style,
                                                             glyphAtlas, glyphStore,
                                                             spriteAtlas, sprite,
                                                             info, fileSource);
        } else if (info.type == SourceType::Raster) {
            new_tile.data = std::make_shared<RasterTileData>(normalized_id, texturePool, info, fileSource);
        } else {
            throw std::runtime_error("source type not implemented");
        }

        new_tile.data->request(worker, loop, map.getState().getPixelRatio(), callback);
        tile_data.emplace(new_tile.data->id, new_tile.data);
    }

    return new_tile.data->state;
}

double Source::getZoom(const TransformState& state) const {
    double offset = std::log(util::tileSize / info.tile_size) / std::log(2);
    offset += (state.getPixelRatio() > 1.0 ? 1 :0);
    return state.getZoom() + offset;
}

int32_t Source::coveringZoomLevel(const TransformState& state) const {
    return std::floor(getZoom(state));
}

std::forward_list<Tile::ID> Source::coveringTiles(const TransformState& state) const {
    int32_t z = coveringZoomLevel(state);

    if (z < info.min_zoom) return {{}};
    if (z > info.max_zoom) z = info.max_zoom;

    // Map four viewport corners to pixel coordinates
    box points = state.cornersToBox(z);
    const vec2<double>& center = points.center;

    std::forward_list<Tile::ID> covering_tiles = Tile::cover(z, points);

    covering_tiles.sort([&center](const Tile::ID& a, const Tile::ID& b) {
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
bool Source::findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain) {
    bool complete = true;
    int32_t z = id.z;
    auto ids = id.children(z + 1);
    for (const Tile::ID& child_id : ids) {
        const TileData::State state = hasTile(child_id);
        if (state == TileData::State::parsed) {
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
bool Source::findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain) {
    for (int32_t z = id.z - 1; z >= minCoveringZoom; --z) {
        const Tile::ID parent_id = id.parent(z);
        const TileData::State state = hasTile(parent_id);
        if (state == TileData::State::parsed) {
            retain.emplace_front(parent_id);
            return true;
        }
    }
    return false;
}

void Source::update(Map& map, uv::worker& worker,
                    util::ptr<Style> style,
                    GlyphAtlas& glyphAtlas, GlyphStore& glyphStore,
                    SpriteAtlas& spriteAtlas, util::ptr<Sprite> sprite,
                    TexturePool& texturePool, FileSource& fileSource, uv_loop_t& loop,
                    std::function<void ()> callback) {
    if (!loaded || map.getTime() <= updated)
        return;

    bool changed = false;

    int32_t zoom = std::floor(getZoom(map.getState()));
    std::forward_list<Tile::ID> required = coveringTiles(map.getState());

    // Determine the overzooming/underzooming amounts.
    int32_t minCoveringZoom = util::clamp<int32_t>(zoom - 10, info.min_zoom, info.max_zoom);
    int32_t maxCoveringZoom = util::clamp<int32_t>(zoom + 1,  info.min_zoom, info.max_zoom);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        const TileData::State state = addTile(map, worker, style,
                                              glyphAtlas, glyphStore,
                                              spriteAtlas, sprite,
                                              fileSource, loop, texturePool,
                                              id, callback);

        if (state != TileData::State::parsed) {
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

        if (state == TileData::State::initial) {
            changed = true;
        }
    }

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    std::set<Tile::ID> retain_data;
    util::erase_if(tiles, [&retain, &retain_data, &changed](std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair) {
        Tile &tile = *pair.second;
        bool obsolete = std::find(retain.begin(), retain.end(), tile.id) == retain.end();
        if (obsolete) {
            changed = true;
        } else {
            retain_data.insert(tile.data->id);
        }
        return obsolete;
    });

    // Remove all the expired pointers from the set.
    util::erase_if(tile_data, [&retain_data](std::pair<const Tile::ID, std::weak_ptr<TileData>> &pair) {
        const util::ptr<TileData> tile = pair.second.lock();
        if (!tile) {
            return true;
        }

        bool obsolete = retain_data.find(tile->id) == retain_data.end();
        if (obsolete) {
            tile->cancel();
            return true;
        } else {
            return false;
        }
    });

    updated = map.getTime();
}

}
