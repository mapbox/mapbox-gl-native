#include <mbgl/util/image.hpp>

#include <QBuffer>
#include <QByteArray>
#include <QImage>

namespace mbgl {

std::string encodePNG(const PremultipliedImage& pre) {
    QImage image(pre.data.get(), pre.width, pre.height,
        QImage::Format_ARGB32_Premultiplied);

    QByteArray array;
    QBuffer buffer(&array);

    buffer.open(QIODevice::WriteOnly);
    image.rgbSwapped().save(&buffer, "PNG");

    return std::string(array.constData(), array.size());
}

PremultipliedImage decodeWebP(const uint8_t*, size_t);

PremultipliedImage decodeImage(const std::string& string) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(string.data());
    const size_t size = string.size();

    // FIXME: Use Qt WebP decoder plugin.
    if (size >= 12) {
        uint32_t riff_magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        uint32_t webp_magic = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
        if (riff_magic == 0x52494646 && webp_magic == 0x57454250) {
            return decodeWebP(data, size);
        }
    }

    QImage image =
        QImage::fromData(data, size)
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    if (image.isNull()) {
        throw std::runtime_error("Unsupported image type");
    }

    auto img = std::make_unique<uint8_t[]>(image.byteCount());
    memcpy(img.get(), image.constBits(), image.byteCount());

    return { size_t(image.width()), size_t(image.height()), std::move(img) };
}

}
