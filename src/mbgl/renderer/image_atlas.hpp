#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/rect.hpp>

#include <mapbox/shelf-pack.hpp>

#include <array>

namespace mbgl {

class ImagePosition {
public:
    ImagePosition(const mapbox::Bin&, const style::Image::Impl&);

    float pixelRatio;
    Rect<uint16_t> textureRect;

    std::array<uint16_t, 2> tl() const {
        return {{
            textureRect.x,
            textureRect.y
        }};
    }

    std::array<uint16_t, 2> br() const {
        return {{
            static_cast<uint16_t>(textureRect.x + textureRect.w),
            static_cast<uint16_t>(textureRect.y + textureRect.h)
        }};
    }

    std::array<float, 2> displaySize() const {
        return {{
            textureRect.w / pixelRatio,
            textureRect.h / pixelRatio,
        }};
    }
};

using ImagePositions = std::map<std::string, ImagePosition>;

class ImageAtlas {
public:
    PremultipliedImage image;
    ImagePositions positions;
};

ImageAtlas makeImageAtlas(const ImageMap&);

} // namespace mbgl
