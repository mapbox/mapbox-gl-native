#include <mbgl/util/premultiply.hpp>

#include <cmath>

namespace mbgl {
namespace util {

PremultipliedImage premultiply(UnassociatedImage&& src) {
    PremultipliedImage dst;

    dst.size = src.size;
    src.size = { 0, 0 };
    dst.data = std::move(src.data);

    uint8_t* data = dst.data.get();
    for (size_t i = 0; i < dst.bytes(); i += 4) {
        uint8_t& r = data[i + 0];
        uint8_t& g = data[i + 1];
        uint8_t& b = data[i + 2];
        uint8_t& a = data[i + 3];
        r = (r * a + 127) / 255;
        g = (g * a + 127) / 255;
        b = (b * a + 127) / 255;
    }

    return dst;
}

UnassociatedImage unpremultiply(PremultipliedImage&& src) {
    UnassociatedImage dst;

    dst.size = src.size;
    src.size = { 0, 0 };
    dst.data = std::move(src.data);

    uint8_t* data = dst.data.get();
    for (size_t i = 0; i < dst.bytes(); i += 4) {
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

    return dst;
}

} // namespace util
} // namespace mbgl
