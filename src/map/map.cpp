#include <llmr/map/map.hpp>
#include <llmr/map/tile.hpp>

#include <iostream>
#include <thread>

using namespace llmr;

map::map(class platform *platform)
    : platform(platform),
      transform(new class transform()),
      painter(new class painter(platform, transform)) {

    transform->setLonLat(13, 50);
    transform->setZoom(3);

    tiles.push_back(new class tile(1, 0, 0));
    platform->request(tiles.back());

    tiles.push_back(new class tile(1, 1, 0));
    platform->request(tiles.back());

    tiles.push_back(new class tile(1, 0, 1));
    platform->request(tiles.back());

    tiles.push_back(new class tile(1, 1, 1));
    platform->request(tiles.back());

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
    platform->restart();
}

void map::moveBy(double dx, double dy) {
    transform->moveBy(dx, dy);
    platform->restart();
}

void map::scaleBy(double ds, double cx, double cy) {
    transform->scaleBy(ds, cx, cy);
    platform->restart();
}


bool map::render() {
    painter->clear();

    for (tile *tile : tiles) {
        // painter->viewport();
        if (tile->loaded) {
            painter->render(tile);
        }
    }

    return false;
}

void map::tileLoaded(tile *tile) {
    platform->restart();
    // fprintf(stderr, "[%8zx] tile loaded %d/%d/%d\n",
    //         std::hash<std::thread::id>()(std::this_thread::get_id()),
    //         tile->z, tile->x, tile->y);
    // schedule rerender
}

void map::tileFailed(tile *tile) {
    fprintf(stderr, "[%8zx] tile failed to load %d/%d/%d\n",
            std::hash<std::thread::id>()(std::this_thread::get_id()),
            tile->z, tile->x, tile->y);
}
