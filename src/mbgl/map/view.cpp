#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>

#include <cassert>

namespace mbgl {

PremultipliedImage View::readStillImage(std::array<uint16_t, 2>) {
    return {};
}

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

} // namespace mbgl
