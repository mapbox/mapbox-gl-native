#include <llmr/map/map.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/util/vec2.hpp>

#include <iostream>
#include <thread>

#include <cmath>

using namespace llmr;

map::map()
    : transform(new class transform()),
      painter(new class painter(transform)) {

    // transform->setLonLat(13, 50);
    // transform->setZoom(3);
}

map::~map() {
    delete transform;
}

void map::setup() {
    painter->setup();
}

void map::resize(uint32_t width, uint32_t height) {
    transform->width = width;
    transform->height = height;
    updateTiles();
    platform::restart(this);
}

void map::moveBy(double dx, double dy) {
    transform->moveBy(dx, dy);
    updateTiles();
    platform::restart(this);
}

void map::scaleBy(double ds, double cx, double cy) {
    transform->scaleBy(ds, cx, cy);
    updateTiles();
    platform::restart(this);
}

void map::rotateBy(double cx, double cy, double sx, double sy, double ex, double ey) {
    transform->rotateBy(cx, cy, sx, sy, ex, ey);
    updateTiles();
    platform::restart(this);
}

void map::updateTiles() {
    // Figure out what tiles we need to load
    int32_t zoom = transform->getZoom();
    int32_t max_dim = pow(2, zoom);

    // Map four viewport corners to pixel coordinates
    box box;
    transform->mapCornersToBox(zoom, box);

    vec2<int32_t> tl, br;
    tl.x = fmax(0, floor(fmin(box.tl.x, box.bl.x)));
    tl.y = fmax(0, floor(fmin(box.tl.y, box.tr.y)));
    br.x = fmin(max_dim, ceil(fmax(box.tr.x, box.br.x)));
    br.y = fmin(max_dim, ceil(fmax(box.bl.y, box.br.y)));

    typedef vec3<int32_t> tile_id;

    // TODO: Discard tiles that are outside the viewport
    std::vector<tile_id> required;
    for (int32_t y = tl.y; y < br.y; y++) {
        for (int32_t x = tl.x; x < br.x; x++) {
            tile_id id; id.z = zoom; id.x = x; id.y = y;
            required.push_back(id);
        }
    }

    // Add existing child/parent tiles if the actual tile is not yet loaded
    // TODO


    // Remove tiles that we definitely don't need, i.e. tiles that are not on
    // the required list.
    for (std::vector<tile::ptr>::iterator it = tiles.begin(); it != tiles.end(); it++) {
        tile::ptr& tile = *it;
        tile_id id; id.z = tile->z; id.x = tile->x; id.y = tile->y;
        std::vector<tile_id>::const_iterator required_it = std::find(required.begin(), required.end(), id);
        bool retain = required_it != required.end();
        if (retain) {
            // We already have the tile; remove it from the list of required tiles.
            required.erase(required_it);
        } else {
            // We don't need this tile, so we can remove it entirely.
            std::vector<tile::ptr>::iterator to_remove = it - 1;
            tile->cancel();
            tiles.erase(it);
            it = to_remove;
        }
    }

    // Required now only contains those tiles that are not yet in the list of
    // tiles, so we should add them all.
    for (tile_id& id : required) {
        tile::ptr tile = std::make_shared<class tile>(id.z, id.x, id.y);
        tiles.push_back(tile);
        platform::request(this, tile);
    }
}

bool map::render() {
    painter->clear();

    for (tile::ptr& tile : tiles) {
        // painter->viewport();
        if (tile->loaded) {
            painter->render(tile);
        }
    }

    return false;
}

void map::tileLoaded(tile::ptr tile) {
    platform::restart(this);
}

void map::tileFailed(tile::ptr tile) {
    // fprintf(stderr, "[%8zx] tile failed to load %d/%d/%d\n",
    //         std::hash<std::thread::id>()(std::this_thread::get_id()),
    //         tile->z, tile->x, tile->y);
}
