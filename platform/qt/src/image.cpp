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


PremultipliedImage decodeImage(const std::string& data) {
    QImage image =
        QImage::fromData(reinterpret_cast<const uint8_t*>(data.data()), data.size())
        .rgbSwapped()
        .convertToFormat(QImage::Format_ARGB32_Premultiplied);

    if (image.isNull()) {
        throw std::runtime_error("Unsupported image type");
    }

    PremultipliedImage pre;
    pre.width = image.width();
    pre.height = image.height();

    auto img = std::make_unique<uint8_t[]>(image.byteCount());
    memcpy(img.get(), image.constBits(), image.byteCount());

    pre.data = std::move(img);

    return pre;
}

}
