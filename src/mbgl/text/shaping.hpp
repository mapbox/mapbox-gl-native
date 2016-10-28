#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class SpriteAtlasElement;
class SymbolFeature;

class PositionedIcon {
public:
    PositionedIcon() = default;
    PositionedIcon(const SpriteAtlasElement& image_,
                   float top_,
                   float bottom_,
                   float left_,
                   float right_,
                   float angle_)
        : image(image_),
          top(top_),
          bottom(bottom_),
          left(left_),
          right(right_),
          angle(angle_) {}

    optional<SpriteAtlasElement> image;
    float top = 0;
    float bottom = 0;
    float left = 0;
    float right = 0;
    float angle = 0;

    explicit operator bool() const { return image && (*image).pos.hasArea(); }
};

PositionedIcon shapeIcon(const SpriteAtlasElement&,
                         const SymbolFeature&);

} // namespace mbgl
