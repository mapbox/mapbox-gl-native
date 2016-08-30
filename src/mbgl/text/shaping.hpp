#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

struct SpriteAtlasElement;

namespace style {
class SymbolLayoutProperties;
} // namespace style

class PositionedIcon {
    public:
        explicit PositionedIcon() {}
        explicit PositionedIcon(const SpriteAtlasElement& _image,
                float _top, float _bottom, float _left, float _right) :
            image(_image), top(_top), bottom(_bottom), left(_left), right(_right) {}

        optional<SpriteAtlasElement> image;
        float top = 0;
        float bottom = 0;
        float left = 0;
        float right = 0;

        operator bool() const { return image && (*image).pos.hasArea(); }
};

PositionedIcon shapeIcon(const SpriteAtlasElement& image, const style::SymbolLayoutProperties&);

} // namespace mbgl
