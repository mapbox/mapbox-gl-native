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

    int stride = width * 4;
    size_t webpSize = stride * height;
    auto webp = std::make_unique<uint8_t[]>(webpSize);

    if (!WebPDecodeRGBAInto(data, size, webp.get(), webpSize, stride)) {
        throw std::runtime_error("failed to decode WebP data");
    }

    UnassociatedImage image({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
                            std::move(webp));
    return util::premultiply(std::move(image));
}

} // namespace mbgl
