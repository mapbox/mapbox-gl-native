#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/exception.hpp>

namespace mbgl {
namespace style {

namespace {

bool validateStretch(const ImageStretches& stretches, const float size) {
    if (stretches.empty()) {
        return true;
    }
    float last = 0;
    for (auto& part : stretches) {
        if (part.first < last || part.second < part.first || size < part.second) {
            return false;
        }
        last = part.second;
    }
    return true;
}

bool validateContent(const ImageContent& content, const Size& size) {
    if (content.left < 0 || size.width < content.left) return false;
    if (content.top < 0 || size.height < content.top) return false;
    if (content.right < 0 || size.width < content.right) return false;
    if (content.bottom < 0 || size.height < content.bottom) return false;
    if (content.right < content.left) return false;
    if (content.bottom < content.top) return false;
    return true;
}

} // namespace

Image::Impl::Impl(std::string id_,
                  PremultipliedImage&& image_,
                  const float pixelRatio_,
                  bool sdf_,
                  ImageStretches stretchX_,
                  ImageStretches stretchY_,
                  optional<ImageContent> content_)
    : id(std::move(id_)),
      image(std::move(image_)),
      pixelRatio(pixelRatio_),
      sdf(sdf_),
      stretchX(std::move(stretchX_)),
      stretchY(std::move(stretchY_)),
      content(std::move(content_)) {
    if (!image.valid()) {
        throw util::StyleImageException("dimensions may not be zero");
    } else if (pixelRatio <= 0) {
        throw util::StyleImageException("pixelRatio may not be <= 0");
    } else if (!validateStretch(stretchX, image.size.width)) {
        throw util::StyleImageException("stretchX is out of bounds or overlapping");
    } else if (!validateStretch(stretchY, image.size.height)) {
        throw util::StyleImageException("stretchY is out of bounds or overlapping");
    } else if (content && !validateContent(*content, image.size)) {
        throw util::StyleImageException("content area is invalid");
    }
}

} // namespace style
} // namespace mbgl
