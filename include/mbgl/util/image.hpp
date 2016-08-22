#pragma once

#include <mbgl/util/noncopyable.hpp>

#include <string>
#include <memory>
#include <algorithm>

namespace mbgl {

enum ImageAlphaMode {
    Unassociated,
    Premultiplied
};

template <ImageAlphaMode Mode>
class Image : private util::noncopyable {
public:
    Image() = default;

    Image(uint16_t w, uint16_t h)
        : width(w),
          height(h),
          data(std::make_unique<uint8_t[]>(size())) {}

    Image(uint16_t w, uint16_t h, std::unique_ptr<uint8_t[]> data_)
        : width(w),
          height(h),
          data(std::move(data_)) {}

    Image(Image&& o)
        : width(o.width),
          height(o.height),
          data(std::move(o.data)) {}

    Image& operator=(Image&& o) {
        width = o.width;
        height = o.height;
        data = std::move(o.data);
        return *this;
    }

    bool operator==(const Image& rhs) const {
        return width == rhs.width && height == rhs.height &&
               std::equal(data.get(), data.get() + size(), rhs.data.get(),
                          rhs.data.get() + rhs.size());
    }

    size_t stride() const { return static_cast<size_t>(width) * 4; }
    size_t size() const { return static_cast<size_t>(width) * height * 4; }

    uint16_t width = 0;
    uint16_t height = 0;
    std::unique_ptr<uint8_t[]> data;
};

using UnassociatedImage = Image<ImageAlphaMode::Unassociated>;
using PremultipliedImage = Image<ImageAlphaMode::Premultiplied>;

// TODO: don't use std::string for binary data.
PremultipliedImage decodeImage(const std::string&);
std::string encodePNG(const PremultipliedImage&);

} // namespace mbgl
