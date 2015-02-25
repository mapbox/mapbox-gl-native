#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>

namespace mbgl {

void View::discard() {
    // no-op
}

void View::swap() {
    // no-op
}

void View::resize(uint16_t width, uint16_t height, float ratio, uint16_t fbWidth, uint16_t fbHeight) {
    if (map) {
        map->resize(width, height, ratio, fbWidth, fbHeight);
    }
}

std::unique_ptr<StillImage> View::readStillImage() {
    return nullptr;
}

void View::notifyMapChange(MapChange, std::chrono::steady_clock::duration) {
    // no-op
}

}
