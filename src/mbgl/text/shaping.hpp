#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/tagged_string.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {

struct AnchorAlignment {
    AnchorAlignment(float horizontal_, float vertical_)
        : horizontalAlign(horizontal_), verticalAlign(vertical_) {
    }

    static AnchorAlignment getAnchorAlignment(style::SymbolAnchorType anchor) {
        float horizontalAlign = 0.5;
        float verticalAlign = 0.5;

        switch (anchor) {
        case style::SymbolAnchorType::Top:
        case style::SymbolAnchorType::Bottom:
        case style::SymbolAnchorType::Center:
            break;
        case style::SymbolAnchorType::Right:
        case style::SymbolAnchorType::TopRight:
        case style::SymbolAnchorType::BottomRight:
            horizontalAlign = 1;
            break;
        case style::SymbolAnchorType::Left:
        case style::SymbolAnchorType::TopLeft:
        case style::SymbolAnchorType::BottomLeft:
            horizontalAlign = 0;
            break;
        }

        switch (anchor) {
        case style::SymbolAnchorType::Left:
        case style::SymbolAnchorType::Right:
        case style::SymbolAnchorType::Center:
            break;
        case style::SymbolAnchorType::Bottom:
        case style::SymbolAnchorType::BottomLeft:
        case style::SymbolAnchorType::BottomRight:
            verticalAlign = 1;
            break;
        case style::SymbolAnchorType::Top:
        case style::SymbolAnchorType::TopLeft:
        case style::SymbolAnchorType::TopRight:
            verticalAlign = 0;
            break;
        }

        return AnchorAlignment(horizontalAlign, verticalAlign);
    }

    float horizontalAlign;
    float verticalAlign;
};

class SymbolFeature;
class BiDi;

class PositionedIcon {
private:
    PositionedIcon(ImagePosition image_,
                   float top_,
                   float bottom_,
                   float left_,
                   float right_,
                   float angle_)
        : _image(std::move(image_)),
          _top(top_),
          _bottom(bottom_),
          _left(left_),
          _right(right_),
          _angle(angle_) {}

    ImagePosition _image;
    float _top;
    float _bottom;
    float _left;
    float _right;
    float _angle;

public:
    static PositionedIcon shapeIcon(const ImagePosition&,
                                    const std::array<float, 2>& iconOffset,
                                    style::SymbolAnchorType iconAnchor,
                                    const float iconRotation);

    const ImagePosition& image() const { return _image; }
    float top() const { return _top; }
    float bottom() const { return _bottom; }
    float left() const { return _left; }
    float right() const { return _right; }
    float angle() const { return _angle; }
};

const Shaping getShaping(const TaggedString& string,
                         float maxWidth,
                         float lineHeight,
                         style::SymbolAnchorType textAnchor,
                         style::TextJustifyType textJustify,
                         float spacing,
                         const Point<float>& translate,
                         float verticalHeight,
                         const WritingModeType,
                         BiDi& bidi,
                         const GlyphMap& glyphs);

} // namespace mbgl
