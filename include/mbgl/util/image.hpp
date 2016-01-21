#ifndef MBGL_UTIL_IMAGE
#define MBGL_UTIL_IMAGE

#include <string>
#include <memory>

namespace mbgl {

enum ImageAlphaMode {
    Unassociated,
    Premultiplied
};

template <ImageAlphaMode Mode>
class Image {
public:
    Image() = default;

    Image(size_t w, size_t h)
        : width(w),
          height(h),
          data(std::make_unique<uint8_t[]>(size())) {}

    Image(size_t w, size_t h, std::unique_ptr<uint8_t[]> data_)
        : width(w),
          height(h),
          data(std::move(data_)) {}

    size_t stride() const { return width * 4; }
    size_t size() const { return width * height * 4; }

    size_t width = 0;
    size_t height = 0;
    std::unique_ptr<uint8_t[]> data;
};

using UnassociatedImage = Image<ImageAlphaMode::Unassociated>;
using PremultipliedImage = Image<ImageAlphaMode::Premultiplied>;

// TODO: don't use std::string for binary data.
PremultipliedImage decodeImage(const std::string&);
std::string encodePNG(const PremultipliedImage&);

} // namespace mbgl

#endif
