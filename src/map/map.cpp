#include <llmr/map/map.hpp>
#include <llmr/map/settings.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>
#include <llmr/map/coverage.hpp>

#include <algorithm>

using namespace llmr;

Map::Map(Settings& settings)
    : settings(settings),
      transform(),
      style(),
      painter(transform, settings, style),
      min_zoom(0),
      max_zoom(14) {
}

Map::~Map() {
    settings.sync();
}

void Map::setup(float pixelRatio) {
    painter.setup();

    pixel_ratio = pixelRatio;

    style.sprite = std::make_shared<Sprite>();
    style.sprite->load(kSpriteURL, pixel_ratio);

    style.load(resources::style, resources::style_size);
    // style.loadJSON((const char *)resources::style, resources::style_size);
}

void Map::loadSprite(const std::string& url) {

}

void Map::loadStyle(const uint8_t *const data, uint32_t bytes) {
    style.load(data, bytes);
    update();
}

void Map::loadSettings() {
    transform.setAngle(settings.angle);
    transform.setScale(settings.scale);
    transform.setLonLat(settings.longitude, settings.latitude);
    style.cascade(transform.getZoom());
    update();
}

void Map::resize(uint32_t width, uint32_t height, uint32_t fb_width, uint32_t fb_height) {
    transform.width = width;
    transform.height = height;
    transform.fb_width = fb_width;
    transform.fb_height = fb_height;
    transform.pixelRatio = pixel_ratio;
    update();
}

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.persist();
}

void Map::startPanning() {
    transform.startPanning();
    platform::restart();
}

void Map::stopPanning() {
    transform.stopPanning();
    platform::restart();
}

void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::startScaling() {
    transform.startScaling();
    platform::restart();
}

void Map::stopScaling() {
    transform.stopScaling();
    platform::restart();
}

void Map::rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(cx, cy, sx, sy, ex, ey, duration);
    update();

    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::startRotating() {
    transform.startRotating();
    platform::restart();
}

void Map::stopRotating() {
    transform.stopRotating();
    platform::restart();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.persist();
}

void Map::getLonLat(double& lon, double& lat) const {
    transform.getLonLat(lon, lat);
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::getLonLatZoom(double& lon, double& lat, double& zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.persist();
}

double Map::getZoom() const {
    return transform.getZoom();
}

void Map::setAngle(double angle, double x, double y, double duration) {
    double dx = 0, dy = 0;

    if (x >= 0 && y >= 0) {
        dx = (transform.width  / 2) - x;
        dy = (transform.height / 2) - y;
        transform.moveBy(dx, dy, duration);
    }

    transform.setAngle(angle, duration);

    if (x >= 0 && y >= 0) {
        transform.moveBy(-dx, -dy, duration);
    }

    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.angle = transform.getAngle();
    settings.persist();
}

double Map::getScale() const {
    return transform.getScale();
}

double Map::getAngle() const {
    return transform.getAngle();
}

void Map::resetNorth() {
    transform.setAngle(0, 0.5);
    update();

    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::resetPosition() {
    transform.setAngle(0);
    transform.setLonLat(0, 0);
    transform.setZoom(0);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.angle = transform.getAngle();
    settings.persist();
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::toggleDebug() {
    settings.debug = !settings.debug;
    update();

    settings.persist();
}

void Map::cancelAnimations() {
    transform.cancelAnimations();
}

void Map::update() {
    updateTiles();
    platform::restart();
}


const Tile *Map::hasTile(const Tile::ID& id) {
    for (const Tile& tile : tiles) {
        if (tile.id == id) {
            return &tile;
        }
    }

    return nullptr;
}

const Tile *Map::addTile(const Tile::ID& id) {
    const Tile *tile = hasTile(id);

    if (tile) {
        return tile;
    }

    tiles.emplace_front(id);
    Tile& new_tile = tiles.front();

    // We couldn't find the tile in the list. Create a new one.
    // Try to find the associated TileData object.
    const Tile::ID normalized_id = Tile::normalize(id);

    auto it = std::find_if(tile_data.begin(), tile_data.end(), [&normalized_id](const std::weak_ptr<TileData>& tile_data) {
        return !tile_data.expired() && tile_data.lock()->id == normalized_id;
    });
    if (it != tile_data.end() && !it->expired()) {
        // There is already a tile data object and it's not yet expired, so we're
        // going to reuse it.
        new_tile.data = it->lock();
    } else {
        new_tile.data = std::make_shared<TileData>(normalized_id, style);
        tile_data.push_front(new_tile.data);
    }
    return &new_tile;
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
bool Map::findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain) {
    bool complete = true;
    int32_t z = id.z;

    auto ids = Tile::children(id, z + 1);
    for (const Tile::ID& child_id : ids) {
        const Tile *tile = hasTile(child_id);
        if (tile && tile->data && tile->data->state == TileData::parsed) {
            retain.emplace_front(tile->id);
        } else {
            complete = false;
            if (z < maxCoveringZoom) {
                // Go further down the hierarchy to find more unloaded children.
                findLoadedChildren(child_id, maxCoveringZoom, retain);
            }
        }
    }
    return complete;
};

/**
 * Find a loaded parent of the given tile.
 *
 * @param id The tile ID that we should find children for.
 * @param minCoveringZoom The minimum zoom level of parents to look for.
 * @param retain An object that we add the found tiles to.
 *
 * @return boolean Whether a parent was found.
 */
bool Map::findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain) {
    for (int32_t z = id.z - 1; z >= minCoveringZoom; --z) {
        const Tile::ID parent_id = Tile::parent(id, z);
        const Tile *tile = hasTile(parent_id);
        if (tile && tile->data && tile->data->state == TileData::parsed) {
            retain.emplace_front(tile->id);
            return true;
        }
    }
    return false;
};


bool Map::updateTiles() {
    bool changed = false;

    // Figure out what tiles we need to load
    int32_t zoom = transform.getZoom();
    if (zoom > max_zoom) zoom = max_zoom;
    if (zoom < min_zoom) zoom = min_zoom;

    int32_t max_covering_zoom = zoom + 1;
    if (max_covering_zoom > max_zoom) max_covering_zoom = max_zoom;

    int32_t min_covering_zoom = zoom - 10;
    if (min_covering_zoom < min_zoom) min_covering_zoom = min_zoom;

    // Map four viewport corners to pixel coordinates
    box box = transform.mapCornersToBox(zoom);

    // Performs a scanline algorithm search that covers the rectangle of the box
    // and sorts them by proximity to the center.
    std::forward_list<Tile::ID> required = llmr::covering_tiles(zoom, box);

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        const Tile *tile = addTile(id);
        assert(tile);

        if (tile && tile->data && tile->data->state != TileData::parsed) {
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

        if (tile && tile->data && tile->data->state == TileData::initial) {
            // If the tile is new, we have to make sure to load it.
            tile->data->request();
            changed = true;
        }
    }

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    tiles.remove_if([&retain, &changed](const Tile & tile) {
        bool obsolete = std::find(retain.begin(), retain.end(), tile.id) == retain.end();
        if (obsolete) {
            changed = true;
        }
        return obsolete;
    });

    // Sort tiles by zoom level, front to back.
    // We're painting front-to-back, so we want to draw more detailed tiles first
    // before filling in other parts with lower zoom levels.
    tiles.sort([](const Tile & a, const Tile & b) {
        return a.id.z > b.id.z;
    });

    // Remove all the expired pointers from the list.
    tile_data.remove_if([](const std::weak_ptr<TileData>& tile_data) {
        return tile_data.expired();
    });

    return changed;
}

bool Map::render() {
    if (transform.needsAnimation()) {
        transform.updateAnimations();
    }

    bool changed = updateTiles();

    painter.clear();

    painter.changeMatrix();

    // First, update all tile matrices with the new transform and render into
    // the stencil buffer.
    uint8_t i = 1;
    painter.prepareClippingMask();
    for (Tile& tile : tiles) {
        if (tile.data && tile.data->state == TileData::parsed) {
            // The position matrix.
            transform.matrixFor(tile.matrix, tile.id);
            matrix::multiply(tile.matrix, painter.projMatrix, tile.matrix);
            tile.clip_id = i++;
            painter.drawClippingMask(tile.matrix, tile.clip_id);
        }
    }
    painter.finishClippingMask();

    for (const Tile& tile : tiles) {
        if (tile.data && tile.data->state == TileData::parsed) {
            painter.render(tile);
        }
    }

    painter.renderMatte();

    return changed || transform.needsAnimation();
}
