#pragma once

#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/tagged_string.hpp>

#include <utility>

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

class Padding {
public:
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;

    explicit operator bool() const { return left != 0 || top != 0 || right != 0 || bottom != 0; }

    bool operator==(const Padding& rhs) const {
        return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
    }
};

class PositionedIcon {
private:
    PositionedIcon(
        ImagePosition image_, float top_, float bottom_, float left_, float right_, const Padding& collisionPadding_)
        : _image(std::move(image_)),
          _top(top_),
          _bottom(bottom_),
          _left(left_),
          _right(right_),
          _collisionPadding(collisionPadding_) {}

    ImagePosition _image;
    float _top;
    float _bottom;
    float _left;
    float _right;
    Padding _collisionPadding;

public:
    static PositionedIcon shapeIcon(const ImagePosition&,
                                    const std::array<float, 2>& iconOffset,
                                    style::SymbolAnchorType iconAnchor);

    // Updates shaped icon's bounds based on shaped text's bounds and provided
    // layout properties.
    void fitIconToText(const Shaping& shapedText,
                       style::IconTextFitType textFit,
                       const std::array<float, 4>& padding,
                       const std::array<float, 2>& iconOffset,
                       float fontScale);

    const ImagePosition& image() const { return _image; }
    float top() const { return _top; }
    float bottom() const { return _bottom; }
    float left() const { return _left; }
    float right() const { return _right; }
    const Padding& collisionPadding() const { return _collisionPadding; }
};

Shaping getShaping(const TaggedString& string,
                   float maxWidth,
                   float lineHeight,
                   style::SymbolAnchorType textAnchor,
                   style::TextJustifyType textJustify,
                   float spacing,
                   const std::array<float, 2>& translate,
                   WritingModeType,
                   BiDi& bidi,
                   const GlyphMap& glyphMap,
                   const GlyphPositions& glyphPositions,
                   const ImagePositions& imagePositions,
                   float layoutTextSize,
                   float layoutTextSizeAtBucketZoomLevel,
                   bool allowVerticalPlacement);

} // namespace mbgl
