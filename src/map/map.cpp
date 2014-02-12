#include <llmr/map/map.hpp>
#include <llmr/map/settings.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/style/resources.hpp>
#include <llmr/style/sprite.hpp>

#include <algorithm>

using namespace llmr;

Map::Map(Settings& settings)
    : settings(settings),
      transform(),
      style(),
      painter(transform, settings, style),
      min_zoom(0),
      max_zoom(14) {

    // TODO: Extract that information from the stylesheet instead of hard coding
    style.sprite = std::make_shared<Sprite>();
    style.sprite->load(kSpriteURL);
}

Map::~Map() {
}

void Map::setup() {
    painter.setup();

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
    transform.pixelRatio = (double)fb_width / (double)width;
    update();
}

void Map::moveBy(double dx, double dy, double duration) {
    transform.moveBy(dx, dy, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.save();
}

void Map::scaleBy(double ds, double cx, double cy, double duration) {
    transform.scaleBy(ds, cx, cy, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.save();
}

void Map::rotateBy(double cx, double cy, double sx, double sy, double ex, double ey, double duration) {
    transform.rotateBy(cx, cy, sx, sy, ex, ey, duration);
    update();

    settings.angle = transform.getAngle();
    settings.save();
}

void Map::setLonLat(double lon, double lat, double duration) {
    transform.setLonLat(lon, lat, duration);
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.save();
}

void Map::getLonLat(double &lon, double &lat) const {
    transform.getLonLat(lon, lat);
}

void Map::setLonLatZoom(double lon, double lat, double zoom, double duration) {
    transform.setLonLatZoom(lon, lat, zoom, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.save();
}

void Map::getLonLatZoom(double &lon, double &lat, double &zoom) const {
    transform.getLonLatZoom(lon, lat, zoom);
}

void Map::setScale(double scale, double cx, double cy, double duration) {
    transform.setScale(scale, cx, cy, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.save();
}

void Map::setZoom(double zoom, double duration) {
    transform.setZoom(zoom, duration);
    style.cascade(transform.getZoom());
    update();

    transform.getLonLat(settings.longitude, settings.latitude);
    settings.scale = transform.getScale();
    settings.save();
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
    settings.save();
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
    settings.save();
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
    settings.save();
}

void Map::resetZoom() {
    setZoom(0);
}

void Map::toggleDebug() {
    settings.debug = !settings.debug;
    update();

    settings.save();
}

void Map::cancelAnimations() {
    transform.cancelAnimations();
}

void Map::update() {
    updateTiles();
    platform::restart(this);
}


Tile::Ptr Map::hasTile(const Tile::ID& id) {
    for (Tile::Ptr& tile : tiles) {
        if (tile->id == id) {
            return tile;
        }
    }

    return Tile::Ptr();
}

Tile::Ptr Map::addTile(const Tile::ID& id) {
    Tile::Ptr tile = hasTile(id);

    if (!tile.get()) {
        // We couldn't find the tile in the list. Create a new one.
        tile = std::make_shared<Tile>(id, style);
        assert(tile);
        // std::cerr << "init " << id.z << "/" << id.x << "/" << id.y << std::endl;
        // std::cerr << "add " << tile->toString() << std::endl;
        tiles.push_front(tile);
    }

    return tile;
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
        const Tile::Ptr& tile = hasTile(child_id);
        if (tile && tile->state == Tile::ready) {
            assert(tile);
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
        const Tile::Ptr tile = hasTile(parent_id);
        if (tile && tile->state == Tile::ready) {
            assert(tile);
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


    int32_t max_dim = pow(2, zoom);

    // Map four viewport corners to pixel coordinates
    box box;
    transform.mapCornersToBox(zoom, box);

    vec2<int32_t> tl, br;
    tl.x = fmax(0, floor(fmin(box.tl.x, box.bl.x)));
    tl.y = fmax(0, floor(fmin(box.tl.y, box.tr.y)));
    br.x = fmin(max_dim, ceil(fmax(box.tr.x, box.br.x)));
    br.y = fmin(max_dim, ceil(fmax(box.bl.y, box.br.y)));


    // TODO: Discard tiles that are outside the viewport
    std::forward_list<Tile::ID> required;
    for (int32_t y = tl.y; y < br.y; ++y) {
        for (int32_t x = tl.x; x < br.x; ++x) {
            required.emplace_front(x, y, zoom);
        }
    }

    // Retain is a list of tiles that we shouldn't delete, even if they are not
    // the most ideal tile for the current viewport. This may include tiles like
    // parent or child tiles that are *already* loaded.
    std::forward_list<Tile::ID> retain(required);

    // Add existing child/parent tiles if the actual tile is not yet loaded
    for (const Tile::ID& id : required) {
        Tile::Ptr tile = addTile(id);
        assert(tile);

        if (tile->state != Tile::ready) {
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

        if (tile->state == Tile::initial) {
            // If the tile is new, we have to make sure to load it.
            tile->request();
            changed = true;
        }
    }

    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    tiles.remove_if([&retain, &changed](const Tile::Ptr& tile) {
        assert(tile);
        bool obsolete = std::find(retain.begin(), retain.end(), tile->id) == retain.end();
        if (obsolete) {
            tile->cancel();
            changed = true;
        }
        return obsolete;
    });

    // Sort tiles by zoom level, front to back.
    // We're painting front-to-back, so we want to draw more detailed tiles first
    // before filling in other parts with lower zoom levels.
    tiles.sort([](const Tile::Ptr& a, const Tile::Ptr& b) {
        return a->id.z > b->id.z;
    });

    return changed;
}

bool Map::render() {
    bool changed = false;
    if (transform.needsAnimation()) {
        transform.updateAnimations();
        changed = updateTiles();
    }

    painter.clear();

    for (Tile::Ptr& tile : tiles) {
        assert(tile);
        if (tile->state == Tile::ready) {
            painter.changeMatrix(tile->id);
            painter.render(tile);
        }
    }

    painter.renderMatte();

    return changed || transform.needsAnimation();
}

void Map::tileLoaded(Tile::Ptr tile) {
    // std::cerr << "loaded " << tile->toString() << std::endl;
    update();
}

void Map::tileFailed(Tile::Ptr tile) {
    // fprintf(stderr, "[%8zx] tile failed to load %d/%d/%d\n",
    //         std::hash<std::thread::id>()(std::this_thread::get_id()),
    //         tile->z, tile->x, tile->y);
}
