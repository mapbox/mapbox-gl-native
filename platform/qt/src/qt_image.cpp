#include <mbgl/util/image.hpp>

#include <QBuffer>
#include <QByteArray>
#include <QImage>

namespace mbgl {

std::string encodePNG(const PremultipliedImage& pre) {
    QImage image(pre.data.get(), pre.size.width, pre.size.height,
        QImage::Format_ARGB32_Premultiplied);

    QByteArray array;
    QBuffer buffer(&array);

    buffer.open(QIODevice::WriteOnly);
    image.rgbSwapped().save(&buffer, "PNG");

    return std::string(array.constData(), array.size());
}

#if !defined(QT_IMAGE_DECODERS)
PremultipliedImage decodeJPEG(const uint8_t*, size_t);
PremultipliedImage decodeWebP(const uint8_t*, size_t);
#endif

PremultipliedImage decodeImage(const std::string& string) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(string.data());
    const size_t size = string.size();

#if !defined(QT_IMAGE_DECODERS)
    if (size >= 12) {
        uint32_t riff_magic = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
        uint32_t webp_magic = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
        if (riff_magic == 0x52494646 && webp_magic == 0x57454250) {
            return decodeWebP(data, size);
        }
    }

    if (size >= 2) {
        uint16_t magic = ((data[0] << 8) | data[1]) & 0xffff;
        if (magic == 0xFFD8) {
            return decodeJPEG(data, size);
        }
    }
#endif

    QImage image =
        QImage::fromData(data, size)
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    if (image.isNull()) {
        throw std::runtime_error("Unsupported image type");
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    auto img = std::make_unique<uint8_t[]>(image.sizeInBytes());
    memcpy(img.get(), image.constBits(), image.sizeInBytes());
#else
    auto img = std::make_unique<uint8_t[]>(image.byteCount());
    memcpy(img.get(), image.constBits(), image.byteCount());
#endif

    return { { static_cast<uint32_t>(image.width()), static_cast<uint32_t>(image.height()) },
             std::move(img) };
}
}
