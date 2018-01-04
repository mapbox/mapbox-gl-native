#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/size.hpp>

#include <string>
#include <cstring>
#include <memory>
#include <algorithm>

namespace mbgl {

enum class ImageAlphaMode {
    Unassociated,
    Premultiplied,
    Exclusive, // Alpha-channel only
};

template <ImageAlphaMode Mode>
class Image : private util::noncopyable {
public:
    Image() = default;

    Image(Size size_)
        : size(std::move(size_)),
          data(std::make_unique<uint8_t[]>(bytes())) {}

    Image(Size size_, const uint8_t* srcData, std::size_t srcLength)
        : size(std::move(size_)) {
        if (srcLength != bytes()) {
            throw std::invalid_argument("mismatched image size");
        }
        data = std::make_unique<uint8_t[]>(bytes());
        std::copy(srcData, srcData + srcLength, data.get());
    }

    Image(Size size_, std::unique_ptr<uint8_t[]> data_)
        : size(std::move(size_)),
          data(std::move(data_)) {}

    Image(Image&& o)
        : size(o.size),
          data(std::move(o.data)) {
        o.size.width = o.size.height = 0;
    }

    Image& operator=(Image&& o) {
        size = o.size;
        data = std::move(o.data);
        o.size.width = o.size.height = 0;
        return *this;
    }

    friend bool operator==(const Image& lhs, const Image& rhs) {
        return std::equal(lhs.data.get(), lhs.data.get() + lhs.bytes(),
                          rhs.data.get(), rhs.data.get() + rhs.bytes());
    }

    friend bool operator!=(const Image& lhs, const Image& rhs) {
        return !(lhs == rhs);
    }

    bool valid() const {
        return !size.isEmpty() && data.get() != nullptr;
    }

    template <typename T = Image>
    T clone() const {
        T copy_(size);
        std::copy(data.get(), data.get() + bytes(), copy_.data.get());
        return copy_;
    }

    size_t stride() const { return channels * size.width; }
    size_t bytes() const { return stride() * size.height; }

    void fill(uint8_t value) {
        std::fill(data.get(), data.get() + bytes(), value);
    }

    void resize(Size size_) {
        if (size == size_) {
            return;
        }
        Image newImage(size_);
        newImage.fill(0);
        copy(*this, newImage, {0, 0}, {0, 0}, {
            std::min(size.width, size_.width),
            std::min(size.height, size_.height)
        });
        operator=(std::move(newImage));
    }

    // Clears the rect area specified by `pt` and `size` from `dstImage`.
    static void clear(Image& dstImg, const Point<uint32_t>& pt, const Size& size) {
        if (size.isEmpty()) {
            return;
        }

        if (!dstImg.valid()) {
            throw std::invalid_argument("invalid destination for image clear");
        }

        if (size.width > dstImg.size.width ||
            size.height > dstImg.size.height ||
            pt.x > dstImg.size.width - size.width ||
            pt.y > dstImg.size.height - size.height) {
            throw std::out_of_range("out of range destination coordinates for image clear");
        }

        uint8_t* dstData = dstImg.data.get();

        for (uint32_t y = 0; y < size.height; y++) {
            const std::size_t dstOffset = (pt.y + y) * dstImg.stride() + pt.x * channels;
            std::memset(dstData + dstOffset, 0, size.width * channels);
        }
    }

    // Copy image data within `rect` from `src` to the rectangle of the same size at `pt`
    // in `dst`. If the specified bounds exceed the bounds of the source or destination,
    // throw `std::out_of_range`. Must not be used to move data within a single Image.
    static void copy(const Image& srcImg, Image& dstImg, const Point<uint32_t>& srcPt, const Point<uint32_t>& dstPt, const Size& size) {
        if (size.isEmpty()) {
            return;
        }

        if (!srcImg.valid()) {
            throw std::invalid_argument("invalid source for image copy");
        }

        if (!dstImg.valid()) {
            throw std::invalid_argument("invalid destination for image copy");
        }

        if (size.width > srcImg.size.width ||
            size.height > srcImg.size.height ||
            srcPt.x > srcImg.size.width - size.width ||
            srcPt.y > srcImg.size.height - size.height) {
            throw std::out_of_range("out of range source coordinates for image copy");
        }

        if (size.width > dstImg.size.width ||
            size.height > dstImg.size.height ||
            dstPt.x > dstImg.size.width - size.width ||
            dstPt.y > dstImg.size.height - size.height) {
            throw std::out_of_range("out of range destination coordinates for image copy");
        }

        const uint8_t* srcData = srcImg.data.get();
              uint8_t* dstData = dstImg.data.get();

        assert(srcData != dstData);

        for (uint32_t y = 0; y < size.height; y++) {
            const std::size_t srcOffset = (srcPt.y + y) * srcImg.stride() + srcPt.x * channels;
            const std::size_t dstOffset = (dstPt.y + y) * dstImg.stride() + dstPt.x * channels;
            std::copy(srcData + srcOffset,
                      srcData + srcOffset + size.width * channels,
                      dstData + dstOffset);
        }
    }

    Size size;
    static constexpr size_t channels = Mode == ImageAlphaMode::Exclusive ? 1 : 4;
    std::unique_ptr<uint8_t[]> data;
};

using UnassociatedImage = Image<ImageAlphaMode::Unassociated>;
using PremultipliedImage = Image<ImageAlphaMode::Premultiplied>;
using AlphaImage = Image<ImageAlphaMode::Exclusive>;

// TODO: don't use std::string for binary data.
PremultipliedImage decodeImage(const std::string&);
std::string encodePNG(const PremultipliedImage&);

} // namespace mbgl
