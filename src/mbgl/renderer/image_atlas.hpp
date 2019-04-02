#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/rect.hpp>

#include <mapbox/shelf-pack.hpp>

#include <array>

namespace mbgl {

namespace gfx {
    class Texture;
    class Context;
} // namespace gfx

class ImageManager;

class ImagePosition {
public:
    ImagePosition(const mapbox::Bin&, const style::Image::Impl&, uint32_t version = 0);

    float pixelRatio;
    Rect<uint16_t> textureRect;
    uint32_t version;

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

    std::array<uint16_t, 4> tlbr() const {
        const auto _tl = tl();
        const auto _br = br();
        return {{ _tl[0], _tl[1], _br[0], _br[1] }};
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
    ImagePositions iconPositions;
    ImagePositions patternPositions;

    void patchUpdatedImages(gfx::Context&, gfx::Texture&, const ImageManager&);
private:
    void patchUpdatedImage(gfx::Context&, gfx::Texture&, ImagePosition& position, const ImageManager& imageManager, const std::string& name, uint16_t version);
};

ImageAtlas makeImageAtlas(const ImageMap&, const ImageMap&, const std::unordered_map<std::string, uint32_t>& versionMap);

} // namespace mbgl
