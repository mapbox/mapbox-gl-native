#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/size.hpp>

#include <string>
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

    Image(Size size_, std::unique_ptr<uint8_t[]> data_)
        : size(std::move(size_)),
          data(std::move(data_)) {}

    Image(Image&& o)
        : size(o.size),
          data(std::move(o.data)) {}

    Image& operator=(Image&& o) {
        size = o.size;
        data = std::move(o.data);
        return *this;
    }

    bool operator==(const Image& rhs) const {
        return size == rhs.size &&
               std::equal(data.get(), data.get() + bytes(), rhs.data.get(),
                          rhs.data.get() + rhs.bytes());
    }

    bool valid() const {
        return size && data.get() != nullptr;
    }

    size_t stride() const { return channels * size.width; }
    size_t bytes() const { return stride() * size.height; }

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
