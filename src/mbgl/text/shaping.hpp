#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class SpriteAtlasElement;
class SymbolFeature;
class BiDi;

class PositionedIcon {
private:
    PositionedIcon(const SpriteAtlasElement& image_,
                   float top_,
                   float bottom_,
                   float left_,
                   float right_,
                   float angle_)
        : _image(image_),
          _top(top_),
          _bottom(bottom_),
          _left(left_),
          _right(right_),
          _angle(angle_) {}

    optional<SpriteAtlasElement> _image;
    float _top;
    float _bottom;
    float _left;
    float _right;
    float _angle;

public:
    static optional<PositionedIcon> shapeIcon(const class SpriteAtlasElement&, const std::array<float, 2>& iconOffset, const float iconRotation);

    optional<class SpriteAtlasElement> image() const { return _image; }
    float top() const { return _top; }
    float bottom() const { return _bottom; }
    float left() const { return _left; }
    float right() const { return _right; }
    float angle() const { return _angle; }
};

const Shaping getShaping(const std::u16string& string,
                         float maxWidth,
                         float lineHeight,
                         float horizontalAlign,
                         float verticalAlign,
                         float justify,
                         float spacing,
                         const Point<float>& translate,
                         float verticalHeight,
                         const WritingModeType,
                         BiDi& bidi,
                         const GlyphPositions& glyphs);

} // namespace mbgl
