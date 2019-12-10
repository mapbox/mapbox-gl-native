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
    Rect<uint16_t> paddedRect;
    uint32_t version;
    style::ImageStretches stretchX;
    style::ImageStretches stretchY;
    style::ImageContent content;

    std::array<uint16_t, 2> tl() const {
        return {{static_cast<uint16_t>(paddedRect.x + padding), static_cast<uint16_t>(paddedRect.y + padding)}};
    }

    std::array<uint16_t, 2> br() const {
        return {{static_cast<uint16_t>(paddedRect.x + paddedRect.w - padding),
                 static_cast<uint16_t>(paddedRect.y + paddedRect.h - padding)}};
    }

    std::array<uint16_t, 4> tlbr() const {
        const auto _tl = tl();
        const auto _br = br();
        return {{ _tl[0], _tl[1], _br[0], _br[1] }};
    }

    std::array<float, 2> displaySize() const {
        return {{
            static_cast<float>(paddedRect.w - padding * 2) / pixelRatio,
            static_cast<float>(paddedRect.h - padding * 2) / pixelRatio,
        }};
    }
};

using ImagePositions = std::map<std::string, ImagePosition>;

class ImagePatch {
public:
    ImagePatch(Immutable<style::Image::Impl> image_, const Rect<uint16_t>& paddedRect_)
        : image(std::move(image_)), paddedRect(paddedRect_) {}

    Immutable<style::Image::Impl> image;
    Rect<uint16_t> paddedRect;
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
