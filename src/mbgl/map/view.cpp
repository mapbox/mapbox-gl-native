#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

void View::resize(uint16_t, uint16_t, float) {
    // no-op
}

std::unique_ptr<StillImage> View::readStillImage() {
    return nullptr;
}

void View::notifyMapChange(MapChange, Duration) {
    // no-op
}


}
