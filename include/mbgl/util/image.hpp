#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/size.hpp>

#include <string>
#include <cstring>
#include <memory>
#include <algorithm>

namespace mbgl {

class ImageBase {
public:
    // Returns true, if has data and non-zero size; returns false otherwise.
    bool valid() const;
    uint8_t* data() { return data_.get(); }
    const uint8_t* data() const { return data_.get(); }
    const Size& size() const { return size_; }
    // Takes data from this image, leaving it in invalid state and with empty size.
    std::unique_ptr<uint8_t[]> takeData();

protected:
    ImageBase();
    ImageBase(const Size& s, size_t channels);
    ImageBase(const Size& s, std::unique_ptr<uint8_t[]> d);
    ImageBase(ImageBase&& o);
    ~ImageBase();
    ImageBase& operator=(ImageBase&& o);

    size_t stride(size_t channels) const;
    size_t bytes(size_t channels) const;
    void resize(const Size& s, size_t channels);
    void clear(const Point<uint32_t>& pt, const Size& size, size_t channels);

    static void copy(const ImageBase& srcImg, ImageBase& dstImg, const Point<uint32_t>& srcPt, const Point<uint32_t>& dstPt, const Size& size, size_t channels);

    Size size_;
    std::unique_ptr<uint8_t[]> data_;
};

enum class ImageAlphaMode {
    Unassociated,
    Premultiplied,
    Exclusive, // Alpha-channel only
};

template <ImageAlphaMode Mode>
class Image : public ImageBase {
public:
    static constexpr size_t channels = Mode == ImageAlphaMode::Exclusive ? 1 : 4;
    Image() = default;
    Image(const Size& s) : ImageBase(s, channels) {}
    Image(const Size& s, std::unique_ptr<uint8_t[]> d) : ImageBase(s, std::move(d)) {}
    Image(Image&& o) = default;
    ~Image() = default;
    Image& operator=(Image&& o) = default;
    size_t stride() const { return ImageBase::stride(channels); }
    size_t bytes() const { return ImageBase::bytes(channels); }
    // Resizes this image. If the given size is more than the current one,
    // extra space is filled with '0'.
    void resize(const Size& s) { ImageBase::resize(s, channels); }
    // Clears the rect area specified by `pt` and `size` from this image.
    // The image must be valid.
    void clear(const Point<uint32_t>& pt, const Size& s) {
        ImageBase::clear(pt, s, channels);
    }
    bool operator==(const Image& o) const {
        return std::equal(data(), data() + bytes(),
                          o.data(), o.data() + o.bytes());
    }
    bool operator!=(const Image& o) const {
        return !(operator==(o));
    }

    template <typename T = Image>
    T clone() const {
        T copy_(size());
        std::copy(data(), data() + bytes(), copy_.data());
        return copy_;
    }

    // Copy image data within `rect` from `src` to the rectangle of the same size at `pt`
    // in `dst`. The specified bounds must not exceed the bounds of the source or destination.
    // Must not be used to move data within a single Image.
    static void copy(const Image& srcImg, Image& dstImg, const Point<uint32_t>& srcPt, const Point<uint32_t>& dstPt, const Size& size) {
        return ImageBase::copy(srcImg, dstImg, srcPt, dstPt, size, channels);
    }
};

using UnassociatedImage = Image<ImageAlphaMode::Unassociated>;
using PremultipliedImage = Image<ImageAlphaMode::Premultiplied>;
using AlphaImage = Image<ImageAlphaMode::Exclusive>;

// TODO: don't use std::string for binary data.
PremultipliedImage decodeImage(const std::string&);
std::string encodePNG(const PremultipliedImage&);

} // namespace mbgl
