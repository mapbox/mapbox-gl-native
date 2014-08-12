#include <mbgl/map/source.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/raster.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/texturepool.hpp>
#include <mbgl/util/vec.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/style/style_layer.hpp>


#include <mbgl/map/vector_tile_data.hpp>
#include <mbgl/map/raster_tile_data.hpp>

namespace mbgl {

Source::Source(SourceInfo info, const std::string &access_token)
    : info(
          info.type,
          normalizeSourceURL(info.url, access_token),
          info.tile_size,
          info.min_zoom,
          info.max_zoom
      ) {}

std::string Source::normalizeSourceURL(const std::string &url, const std::string &access_token) {
    const std::string t = "mapbox://";
    if (url.compare(0, t.length(), t) == 0) {
        if (access_token.empty()) {
            throw std::runtime_error("You must provide a Mapbox API access token for Mapbox tile sources");
        }
        return std::string("https://api.tiles.mapbox.com/v4/") + url.substr(t.length()) + "/{z}/{x}/{y}.vector.pbf?access_token=" + access_token;
    } else {
        return url;
    }
}

bool Source::update(Map &map) {
    if (map.getTime() > updated) {
        return updateTiles(map);
    } else {
        return false;
    }
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
        gl::group group(util::sprintf<32>("mask %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
        painter.drawClippingMask(tile.matrix, tile.clip);
    }
}

void Source::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc) {
    gl::group group(std::string("layer: ") + layer_desc->id);
    for (const std::pair<const Tile::ID, std::unique_ptr<Tile>> &pair : tiles) {
        Tile &tile = *pair.second;
        if (tile.data && tile.data->state == TileData::State::parsed) {
            painter.renderTileLayer(tile, layer_desc, tile.matrix);
        }
    }
}

void Source::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix) {
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

TileData::State Source::addTile(Map &map, const Tile::ID& id) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    auto pos = tiles.emplace(id, std::make_unique<Tile>(id));
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
            new_tile.data = std::make_shared<VectorTileData>(normalized_id, map, info);
        } else if (info.type == SourceType::Raster) {
            new_tile.data = std::make_shared<RasterTileData>(normalized_id, map, info);
        } else {
            throw std::runtime_error("source type not implemented");
        }

        new_tile.data->request();
        tile_data.emplace(new_tile.data->id, new_tile.data);
    }

    return new_tile.data->state;
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

bool Source::updateTiles(Map &map) {
    bool changed = false;

    // Figure out what tiles we need to load
    int32_t clamped_zoom = map.getState().getIntegerZoom();
    if (clamped_zoom > info.max_zoom) clamped_zoom = info.max_zoom;
    if (clamped_zoom < info.min_zoom) clamped_zoom = info.min_zoom;

    int32_t max_covering_zoom = clamped_zoom + 1;
    if (max_covering_zoom > info.max_zoom) max_covering_zoom = info.max_zoom;

    int32_t min_covering_zoom = clamped_zoom - 10;
    if (min_covering_zoom < info.min_zoom) min_covering_zoom = info.min_zoom;

    // Map four viewport corners to pixel coordinates
    box box = map.getState().cornersToBox(clamped_zoom);

    // Performs a scanline algorithm search that covers the rectangle of the box
    // and sorts them by proximity to the center.

    std::forward_list<Tile::ID> required = covering_tiles(map.getState(), clamped_zoom, box);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        const TileData::State state = addTile(map, id);

        if (state != TileData::State::parsed) {
//            if (use_raster && (transform.rotating || transform.scaling || transform.panning))
//                break;

            // The tile we require is not yet loaded. Try to find a parent or
            // child tile that we already have.

            // First, try to find existing child tiles that completely cover the
            // missing tile.
            bool complete = findLoadedChildren(id, max_covering_zoom, retain);

            // Then, if there are no complete child tiles, try to find existing
            // parent tiles that completely cover the missing tile.
            if (!complete) {
                findLoadedParent(id, min_covering_zoom, retain);
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
        const std::shared_ptr<TileData> tile = pair.second.lock();
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

    return changed;
}

// Taken from polymaps src/Layer.js
// https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383

struct edge {
    double x0 = 0, y0 = 0;
    double x1 = 0, y1 = 0;
    double dx = 0, dy = 0;
    edge(double x0, double y0, double x1, double y1, double dx, double dy)
    : x0(x0), y0(y0), x1(x1), y1(y1), dx(dx), dy(dy) {}
};

typedef const std::function<void(int32_t, int32_t, int32_t, int32_t)> ScanLine;

// scan-line conversion
edge _edge(const mbgl::vec2<double> a, const mbgl::vec2<double> b) {
    if (a.y > b.y) {
        return { b.x, b.y, a.x, a.y, a.x - b.x, a.y - b.y };
    } else {
        return { a.x, a.y, b.x, b.y, b.x - a.x, b.y - a.y };
    }
}

// scan-line conversion
void _scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = std::fmax(ymin, std::floor(e1.y0)),
    y1 = std::fmin(ymax, std::ceil(e1.y1));

    // sort edges by x-coordinate
    if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ?
        (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) :
        (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0)) {
        std::swap(e0, e1);
    }

    // scan lines!
    double m0 = e0.dx / e0.dy,
    m1 = e1.dx / e1.dy,
    d0 = e0.dx > 0, // use y + 1 to compute x0
    d1 = e1.dx < 0; // use y + 1 to compute x1
    for (int32_t y = y0; y < y1; y++) {
        double x0 = m0 * std::fmax(0, std::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0,
        x1 = m1 * std::fmax(0, std::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(std::floor(x1), std::ceil(x0), y, ymax);
    }
}

// scan-line conversion
void _scanTriangle(const mbgl::vec2<double> a, const mbgl::vec2<double> b, const mbgl::vec2<double> c, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
    edge ab = _edge(a, b);
    edge bc = _edge(b, c);
    edge ca = _edge(c, a);

    // sort edges by y-length
    if (ab.dy > bc.dy) { std::swap(ab, bc); }
    if (ab.dy > ca.dy) { std::swap(ab, ca); }
    if (bc.dy > ca.dy) { std::swap(bc, ca); }

    // scan span! scan span!
    if (ab.dy) _scanSpans(ca, ab, ymin, ymax, scanLine);
    if (bc.dy) _scanSpans(ca, bc, ymin, ymax, scanLine);
}

double Source::getZoom(const TransformState &state) const {
    double offset = log(util::tileSize / info.tile_size) / log(2);
    offset += (state.getPixelRatio() > 1.0 ? 1 :0);
    return state.getZoom() + offset;
}

std::forward_list<mbgl::Tile::ID> Source::covering_tiles(const TransformState &state, int32_t clamped_zoom, const box& points) {
    int32_t dim = std::pow(2, clamped_zoom);
    std::forward_list<mbgl::Tile::ID> tiles;
    bool is_raster = (info.type == SourceType::Raster);
    double search_zoom = getZoom(state);

    auto scanLine = [&tiles, clamped_zoom, is_raster, search_zoom](int32_t x0, int32_t x1, int32_t y, int32_t ymax) {
        int32_t x;
        if (y >= 0 && y <= ymax) {
            for (x = x0; x < x1; x++) {
                if (is_raster) {
                    Tile::ID id = Tile::ID(clamped_zoom, x, y);
                    auto ids = id.children(search_zoom);
                    for (const Tile::ID& child_id : ids) {
                        tiles.emplace_front(child_id.z, child_id.x, child_id.y);
                    }
                } else {
                    tiles.emplace_front(clamped_zoom, x, y);
                }
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    _scanTriangle(points.tl, points.tr, points.br, 0, dim, scanLine);
    _scanTriangle(points.br, points.bl, points.tl, 0, dim, scanLine);

    const vec2<double>& center = points.center;
    tiles.sort([&center](const Tile::ID& a, const Tile::ID& b) {
        // Sorts by distance from the box center
        return std::fabs(a.x - center.x) + std::fabs(a.y - center.y) <
        std::fabs(b.x - center.x) + std::fabs(b.y - center.y);
    });

    tiles.unique();

    return tiles;
}

}
