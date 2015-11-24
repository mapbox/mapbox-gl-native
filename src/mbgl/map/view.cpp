#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>

#include <cassert>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

UnassociatedImage View::readStillImage() {
    return {};
}

void View::notifyMapChange(MapChange) {
    // no-op
}


}
