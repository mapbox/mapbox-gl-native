#include <llmr/map/source.hpp>
#include <llmr/map/map.hpp>
#include <llmr/map/transform.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/util/constants.hpp>
#include <llmr/util/raster.hpp>
#include <llmr/util/string.hpp>
#include <llmr/util/texturepool.hpp>
#include <llmr/util/vec.hpp>
#include <llmr/geometry/glyph_atlas.hpp>


#include <llmr/map/vector_tile_data.hpp>
#include <llmr/map/raster_tile_data.hpp>

using namespace llmr;

Source::Source(Map &map, Painter &painter,
               const char *url, Source::Type type, std::vector<uint32_t> zooms, uint32_t tile_size,
               uint32_t min_zoom, uint32_t max_zoom, bool enabled)
    : enabled(enabled),
      map(map),
      painter(painter),
      type(type),
      zooms(zooms),
      url(url),
      tile_size(tile_size),
      min_zoom(min_zoom),
      max_zoom(max_zoom) {}

bool Source::update() {
    return updateTiles();
}

void Source::updateClipIDs(const std::map<Tile::ID, ClipID> &mapping) {
    std::for_each(tiles.begin(), tiles.end(), [&mapping](Tile &tile) {
        auto it = mapping.find(tile.id);
        if (it != mapping.end()) {
            tile.clip = it->second;
        } else {
            tile.clip = ClipID {};
        }
    });
}

size_t Source::prepareRender(const TransformState &transform) {
    if (!enabled) return 0;

    size_t masks = 0;
    for (Tile& tile : tiles) {
        gl::group group(util::sprintf<32>("mask %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
        transform.matrixFor(tile.matrix, tile.id);
        matrix::multiply(tile.matrix, painter.projMatrix, tile.matrix);
        painter.drawClippingMask(tile.matrix, tile.clip);
        masks++;
    }

    return masks;
}

void Source::render(const LayerDescription& layer_desc, const BucketDescription &bucket_desc) {
    if (!enabled) return;

    gl::group group(std::string("layer: ") + layer_desc.name);
    for (const Tile& tile : tiles) {
        if (tile.data && tile.data->state == TileData::State::parsed) {
            painter.renderTileLayer(tile, layer_desc);
        }
    }
}

void Source::finishRender() {
    if (!enabled) return;

    for (const Tile& tile : tiles) {
        painter.renderTileDebug(tile);
    }
}


std::forward_list<Tile::ID> Source::getIDs() const {
    std::forward_list<Tile::ID> ptrs;
    std::transform(tiles.begin(), tiles.end(), std::front_inserter(ptrs), [](const Tile &tile) {
        return tile.id;
    });
    return ptrs;
}

TileData::State Source::hasTile(const Tile::ID& id) {
    for (const Tile& tile : tiles) {
        if (tile.id == id && tile.data) {
            return tile.data->state;
        }
    }

    return TileData::State::invalid;
}

TileData::State Source::addTile(const Tile::ID& id) {
    const TileData::State state = hasTile(id);

    if (state != TileData::State::invalid) {
        return state;
    }

    tiles.emplace_front(id);
    Tile& new_tile = tiles.front();

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const Tile::ID normalized_id = id.normalized();

    auto it = std::find_if(tile_data.begin(), tile_data.end(), [normalized_id](const std::weak_ptr<TileData>& tile_data) {
        return !tile_data.expired() && tile_data.lock()->id == normalized_id;
    });

    if (it != tile_data.end()) {
        // Create a shared_ptr handle. Note that this might be empty!
        new_tile.data = it->lock();
    }

    if (new_tile.data && new_tile.data->state == TileData::State::obsolete) {
        // Do not consider the tile if it's already obsolete.
        new_tile.data.reset();
    }

    if (!new_tile.data) {
        // If we don't find working tile data, we're just going to load it.

        std::string formed_url;

        if (type == Source::Type::vector) {
            formed_url = util::sprintf<256>(url, normalized_id.z, normalized_id.x, normalized_id.y);
            new_tile.data = std::make_shared<VectorTileData>(normalized_id, map, formed_url);
        } else {
            formed_url = util::sprintf<256>(url, normalized_id.z, normalized_id.x, normalized_id.y, (map.getState().getPixelRatio() > 1.0 ? "@2x" : ""));
            new_tile.data = std::make_shared<RasterTileData>(normalized_id, map, formed_url);
        }

        new_tile.data->request();
        tile_data.push_front(new_tile.data);
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

bool Source::updateTiles() {
    bool changed = false;

    // Figure out what tiles we need to load
    int32_t zoom = map.getState().getIntegerZoom();
    if (zoom > max_zoom) zoom = max_zoom;
    if (zoom < min_zoom) zoom = min_zoom;

    int32_t max_covering_zoom = zoom + 1;
    if (max_covering_zoom > max_zoom) max_covering_zoom = max_zoom;

    int32_t min_covering_zoom = zoom - 10;
    if (min_covering_zoom < min_zoom) min_covering_zoom = min_zoom;

    // Map four viewport corners to pixel coordinates
    box box = map.getState().cornersToBox(zoom);

    // Performs a scanline algorithm search that covers the rectangle of the box
    // and sorts them by proximity to the center.

    std::forward_list<Tile::ID> required = covering_tiles(zoom, box);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        const TileData::State state = addTile(id);

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
    std::forward_list<Tile::ID> retain_data;
    tiles.remove_if([&retain, &retain_data, &changed](const Tile & tile) {
        bool obsolete = std::find(retain.begin(), retain.end(), tile.id) == retain.end();
        if (obsolete) {
            changed = true;
        } else {
            retain_data.push_front(tile.data->id);
        }
        return obsolete;
    });

    // Sort tiles by zoom level, front to back.
    // We're painting front-to-back, so we want to draw more detailed tiles first
    // before filling in other parts with lower zoom levels.
    tiles.sort([](const Tile & a, const Tile & b) {
        return a.id.z < b.id.z;
    });

    // Remove all the expired pointers from the list.
    tile_data.remove_if([&retain_data](const std::weak_ptr<TileData>& tile_data) {
        const std::shared_ptr<TileData> tile = tile_data.lock();
        if (!tile) {
            return true;
        }
        bool obsolete = std::find(retain_data.begin(), retain_data.end(), tile->id) == retain_data.end();
        if (obsolete) {
            tile->cancel();
            return true;
        } else {
            return false;
        }
    });

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
edge _edge(const llmr::vec2<double> a, const llmr::vec2<double> b) {
    if (a.y > b.y) {
        return { b.x, b.y, a.x, a.y, a.x - b.x, a.y - b.y };
    } else {
        return { a.x, a.y, b.x, b.y, b.x - a.x, b.y - a.y };
    }
}

// scan-line conversion
void _scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine scanLine) {
    double y0 = fmax(ymin, floor(e1.y0)),
    y1 = fmin(ymax, ceil(e1.y1));

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
        double x0 = m0 * fmax(0, fmin(e0.dy, y + d0 - e0.y0)) + e0.x0,
        x1 = m1 * fmax(0, fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(floor(x1), ceil(x0), y, ymax);
    }
}

// scan-line conversion
void _scanTriangle(const llmr::vec2<double> a, const llmr::vec2<double> b, const llmr::vec2<double> c, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
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

double Source::getZoom() const {
    double offset = log(util::tileSize / tile_size) / log(2);
    offset += (map.getState().getPixelRatio() > 1.0 ? 1 :0);
    return map.getState().getZoom() + offset;
}

std::forward_list<llmr::Tile::ID> Source::covering_tiles(int32_t zoom, const box& points) {
    int32_t dim = pow(2, zoom);
    std::forward_list<llmr::Tile::ID> tiles;
    bool is_raster = (type == Type::raster);
    double search_zoom = getZoom();

    auto scanLine = [&tiles, zoom, is_raster, search_zoom](int32_t x0, int32_t x1, int32_t y, int32_t ymax) {
        int32_t x;
        if (y >= 0 && y <= ymax) {
            for (x = x0; x < x1; x++) {
                if (is_raster) {
                    Tile::ID id = Tile::ID(zoom, x, y);
                    auto ids = id.children(search_zoom);
                    for (const Tile::ID& child_id : ids) {
                        tiles.emplace_front(child_id.z, child_id.x, child_id.y);
                    }
                } else {
                    tiles.emplace_front(zoom, x, y);
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
        return fabs(a.x - center.x) + fabs(a.y - center.y) <
        fabs(b.x - center.x) + fabs(b.y - center.y);
    });

    tiles.unique();

    return tiles;
}
