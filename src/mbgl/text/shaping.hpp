#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/tagged_string.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

struct AnchorAlignment {
    AnchorAlignment(float horizontal, float vertical)
        : horizontalAlign(horizontal), verticalAlign(vertical) {
    }

    static AnchorAlignment getAnchorAlignment(style::SymbolAnchorType anchor);

    float horizontalAlign;
    float verticalAlign;
};

// Choose the justification that matches the direction of the TextAnchor
style::TextJustifyType getAnchorJustification(style::SymbolAnchorType anchor);

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

    // Updates shaped icon's bounds based on shaped text's bounds and provided
    // layout properties.
    void fitIconToText(const style::SymbolLayoutProperties::Evaluated& layout,
                       const Shaping& shapedText,
                       float layoutTextSize);

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
                         const WritingModeType,
                         BiDi& bidi,
                         const GlyphMap& glyphs,
                         bool allowVerticalPlacement);

} // namespace mbgl
