#pragma once

#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/rect.hpp>

#include <mapbox/shelf-pack.hpp>

#include <array>

namespace mbgl {

namespace gfx {
class UploadPass;
class Texture;
} // namespace gfx

class ImageManager;

class ImagePosition {
public:
    ImagePosition(const mapbox::Bin&, const style::Image::Impl&, uint32_t version = 0);

    static constexpr const uint16_t padding = 1u;
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

    Rect<uint16_t> paddedTextureRect() const {
        return {static_cast<uint16_t>(textureRect.x - padding),
                static_cast<uint16_t>(textureRect.y - padding),
                static_cast<uint16_t>(textureRect.w + padding * 2),
                static_cast<uint16_t>(textureRect.h + padding * 2)};
    }
};

using ImagePositions = std::map<std::string, ImagePosition>;

class ImagePatch {
public:
    ImagePatch(Immutable<style::Image::Impl> image_,
               const Rect<uint16_t>& textureRect_)
        : image(std::move(image_))
        , textureRect(textureRect_) {}
    Immutable<style::Image::Impl> image;
    Rect<uint16_t> textureRect;
};

class ImageAtlas {
public:
    PremultipliedImage image;
    ImagePositions iconPositions;
    ImagePositions patternPositions;

    std::vector<ImagePatch> getImagePatchesAndUpdateVersions(const ImageManager&);
};

ImageAtlas makeImageAtlas(const ImageMap&, const ImageMap&, const std::unordered_map<std::string, uint32_t>& versionMap);

} // namespace mbgl
