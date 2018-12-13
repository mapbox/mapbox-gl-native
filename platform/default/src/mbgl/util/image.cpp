#include <mbgl/util/image.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/premultiply.hpp>

namespace mbgl {

PremultipliedImage decodePNG(const uint8_t*, size_t);
PremultipliedImage decodeJPEG(const uint8_t*, size_t);

PremultipliedImage decodeImage(const std::string& string) {
    const auto* data = reinterpret_cast<const uint8_t*>(string.data());
    const size_t size = string.size();

    if (size >= 4) {
        uint32_t magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        if (magic == 0x89504E47U) {
            return decodePNG(data, size);
        }
    }

    if (size >= 2) {
        uint16_t magic = ((data[0] << 8) | data[1]) & 0xffff;
        if (magic == 0xFFD8) {
            return decodeJPEG(data, size);
        }
    }

    throw std::runtime_error("unsupported image type");
}

} // namespace mbgl
