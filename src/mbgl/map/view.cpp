#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

PremultipliedImage View::readStillImage(std::array<uint16_t, 2>) {
    return {};
}

void View::notifyMapChange(MapChange) {
    // no-op
}


} // namespace mbgl
