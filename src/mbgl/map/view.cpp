#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

void View::discard() {
    // no-op
}

std::unique_ptr<StillImage> View::readStillImage() {
    return nullptr;
}

void View::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    assert(map);
    map->resize(width, height, ratio, fbWidth, fbHeight);
}

void View::notifyMapChange(MapChange, Duration) {
    // no-op
}


}
