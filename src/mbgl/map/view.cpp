#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

std::shared_ptr<const PremultipliedImage> View::readStillImage() {
    return nullptr;
}

void View::notifyMapChange(MapChange) {
    // no-op
}


} // namespace mbgl
