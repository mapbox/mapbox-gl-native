#include <mbgl/util/premultiply.hpp>

namespace mbgl {
namespace util {

void premultiply(uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; i += 4) {
        uint8_t& r = data[i + 0];
        uint8_t& g = data[i + 1];
        uint8_t& b = data[i + 2];
        uint8_t& a = data[i + 3];
        r = (r * a + 127) / 255;
        g = (g * a + 127) / 255;
        b = (b * a + 127) / 255;
    }
}

void unpremultiply(uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; i += 4) {
        uint8_t& r = data[i + 0];
        uint8_t& g = data[i + 1];
        uint8_t& b = data[i + 2];
        uint8_t& a = data[i + 3];
        if (a) {
            r = (255 * r + (a / 2)) / a;
            g = (255 * g + (a / 2)) / a;
            b = (255 * b + (a / 2)) / a;
        }
    }
}

} // namespace util
} // namespace mbgl
