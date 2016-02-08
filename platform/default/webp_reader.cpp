#include <mbgl/util/image.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/platform/log.hpp>

extern "C"
{
#include <webp/decode.h>
}

namespace mbgl {

PremultipliedImage decodeWebP(const uint8_t* data, size_t size) {
    int width = 0, height = 0;
    if (WebPGetInfo(data, size, &width, &height) == 0) {
        throw std::runtime_error("failed to retrieve WebP basic header information");
    }

    std::unique_ptr<uint8_t[]> webp(WebPDecodeRGBA(data, size, &width, &height));
    if (!webp) {
        throw std::runtime_error("failed to decode WebP data");
    }

    UnassociatedImage image { size_t(width), size_t(height), std::move(webp) };
    return util::premultiply(std::move(image));
}

} // namespace mbgl
