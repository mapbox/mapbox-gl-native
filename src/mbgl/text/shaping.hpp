#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class SpriteAtlasElement;

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

        explicit operator bool() const { return image && (*image).pos.hasArea(); }
};

PositionedIcon shapeIcon(const SpriteAtlasElement& image, const style::SymbolLayoutProperties::Evaluated&);

} // namespace mbgl
