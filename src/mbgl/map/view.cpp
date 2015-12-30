#include <mbgl/map/view.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

PremultipliedImage View::readStillImage() {
    return {};
}

void View::notifyMapChange(MapChange) {
    // no-op
}

mbgl::EdgeInsets getInsets() {
    return {0, 0, 0, 0};
}

} // namespace mbgl
