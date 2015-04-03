#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

void View::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    assert(map);
    map->resize(width, height, ratio, fbWidth, fbHeight);
}

void View::notifyMapChange(MapChange, Duration) {
    // no-op
}


}
