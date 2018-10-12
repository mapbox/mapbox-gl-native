#include <mbgl/util/image.hpp>

namespace mbgl {

ImageBase::ImageBase() = default;

ImageBase::ImageBase(const Size& s, size_t channels) 
    : size_(std::move(s)),
      data_(std::make_unique<uint8_t[]>(bytes(channels))) {
}

ImageBase::ImageBase(const Size& s, std::unique_ptr<uint8_t[]> d)
    : size_(std::move(s)),
      data_(std::move(d)) {}

ImageBase::ImageBase(ImageBase&& o)
    : size_(std::move(o.size_)),
      data_(o.takeData()) {}

ImageBase& ImageBase::operator=(ImageBase&& o) {
    size_ = o.size_;
    data_ = o.takeData();
    return *this;
}

ImageBase::~ImageBase() {}

bool ImageBase::valid() const {
    return !size_.isEmpty() && data_ != nullptr;
}

size_t ImageBase::stride(size_t channels) const {
    return channels * size_.width;
}

size_t ImageBase::bytes(size_t channels) const {
    return channels * size_.area();
}

std::unique_ptr<uint8_t[]> ImageBase::takeData() {
    size_ = Size{};
    return std::move(data_);
}

void ImageBase::resize(const Size& s, size_t channels) {
    if (s == size_) {
        return;
    }   
    const size_t bytesCount = channels * s.area();
    if (bytesCount == 0u) {
        takeData().reset();
        return;
    }
    std::unique_ptr<uint8_t[]> newData{std::make_unique<uint8_t[]>(bytesCount)};
    std::memset(newData.get(), 0, bytesCount);
    ImageBase newImage(s, std::move(newData));
    if (valid() && newImage.valid()) {
        copy(*this, newImage, {0u, 0u}, {0u, 0u}, {
            std::min(s.width, size_.width),
            std::min(s.height, size_.height)
        }, channels);
    }
    operator=(std::move(newImage));
}

void ImageBase::clear(const Point<uint32_t>& pt, const Size& clearedSize, size_t channels) {
    assert(valid());
    if (clearedSize.isEmpty()) {
        return;
    }
    assert(clearedSize.width <= size_.width);
    assert(clearedSize.height <= size_.height);
    assert(pt.x <= size_.width - clearedSize.width);
    assert(pt.y <= size_.height - clearedSize.height);

    for (uint32_t y = 0u; y < clearedSize.height; ++y) {
        const std::size_t dstOffset = (pt.y + y) * stride(channels) + pt.x * channels;
        std::memset(data() + dstOffset, 0u, clearedSize.width * channels);
    }
}
// static
void ImageBase::copy(const ImageBase& srcImg, ImageBase& dstImg, const Point<uint32_t>& srcPt, const Point<uint32_t>& dstPt, const Size& size, size_t channels) {
    assert(srcImg.valid());
    assert(dstImg.valid());

    assert(size.width <= srcImg.size().width);
    assert(size.height <= srcImg.size().height);
    assert(srcPt.x <= srcImg.size().width - size.width);
    assert(srcPt.y <= srcImg.size().height - size.height);

    assert(size.width <= dstImg.size().width);
    assert(size.height <= dstImg.size().height);
    assert(dstPt.x <= dstImg.size().width - size.width);
    assert(dstPt.y <= dstImg.size().height - size.height);

    const uint8_t* srcData = srcImg.data();
    uint8_t* dstData = dstImg.data();

    assert(srcData != dstData);

    for (uint32_t y = 0u; y < size.height; ++y) {
        const std::size_t srcOffset = (srcPt.y + y) * srcImg.stride(channels) + srcPt.x * channels;
        const std::size_t dstOffset = (dstPt.y + y) * dstImg.stride(channels) + dstPt.x * channels;
        std::copy(srcData + srcOffset,
                  srcData + srcOffset + size.width * channels,
                  dstData + dstOffset);
    }
}

} // namespace mbgl
