#include <mbgl/geometry/line_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>

using namespace mbgl;

LineAtlas::LineAtlas(uint16_t width, uint16_t height)
    : width(width),
      height(height),
      dirty(true) {
}

LineAtlas::~LineAtlas() {
}
