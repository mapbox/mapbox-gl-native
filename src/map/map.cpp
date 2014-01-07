#include <llmr/map/map.hpp>
#include <llmr/map/tile.hpp>

#include <iostream>
#include <thread>

using namespace llmr;

map::map(class platform *platform, class painter *painter)
    : platform(platform),
      painter(painter),
      transform(new class transform()) {

    painter->setTransform(transform);


    tile *tile = new class tile(0, 0, 0);
    tiles.push_back(tile);
    platform->request(tile);

}

map::~map() {
    delete transform;
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