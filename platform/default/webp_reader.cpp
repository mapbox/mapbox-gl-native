#include <mbgl/util/image.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/util/logging.hpp>

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

    PremultipliedImage image({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    if (!WebPDecodeRGBAInto(data, size, image.data(), image.bytes(), image.stride())) {
        throw std::runtime_error("failed to decode WebP data");
    }

    // libwebp reads unassociated (= non-premultiplied) data by default, so we have to manually
    // ensure that it gets premultiplied.
    util::premultiply(image.data(), image.bytes());

    return image;
}

} // namespace mbgl
