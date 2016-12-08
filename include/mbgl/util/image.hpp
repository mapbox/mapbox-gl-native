#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/size.hpp>

#include <string>
#include <memory>
#include <algorithm>

namespace mbgl {

enum class ImageAlphaMode {
    Premultiplied, // Premultiplied RGBA
    Exclusive, // Alpha-channel only
};

template <ImageAlphaMode Mode>
class Image : private util::noncopyable {
public:
    Image() = default;

    bool operator==(const Image& rhs) const {
        return size == rhs.size &&
               std::equal(data(), data() + bytes(), rhs.data(), rhs.data() + rhs.bytes());
    }

    bool valid() const {
        return size && data();
    }

    static constexpr size_t channels() { return Mode == ImageAlphaMode::Exclusive ? 1 : 4; }
    size_t stride() const { return channels() * size.width; }
    size_t bytes() const { return stride() * size.height; }

public:
    Size size;

// Implementation-specific code.
public:
    Image(Size size_)
        : size(size_),
          blob_(std::make_unique<uint8_t[]>(bytes())) {}

    Image(Image&& o)
        : size(o.size),
          blob_(std::move(o.blob_)) {}

    Image& operator=(Image&& o) {
        size = o.size;
        blob_ = std::move(o.blob_);
        return *this;
    }

    uint8_t* data() const { return blob_.get(); }

private:
    std::unique_ptr<uint8_t[]> blob_;
};

using PremultipliedImage = Image<ImageAlphaMode::Premultiplied>;
using AlphaImage = Image<ImageAlphaMode::Exclusive>;

// TODO: don't use std::string for binary data.
PremultipliedImage decodeImage(const std::string&);
std::string encodePNG(const PremultipliedImage&);

} // namespace mbgl
