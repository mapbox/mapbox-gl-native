#ifndef MBGL_SYMBOL_LAYER
#define MBGL_SYMBOL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class SpriteAtlas;

class SymbolLayoutProperties {
public:
    LayoutProperty<SymbolPlacementType> symbolPlacement { SymbolPlacementType::Point };
    LayoutProperty<float> symbolSpacing { 250.0f };
    LayoutProperty<bool> symbolAvoidEdges { false };

    LayoutProperty<bool> iconAllowOverlap { false };
    LayoutProperty<bool> iconIgnorePlacement { false };
    LayoutProperty<bool> iconOptional { false };
    LayoutProperty<RotationAlignmentType> iconRotationAlignment { RotationAlignmentType::Viewport };
    LayoutProperty<float> iconSize { 1.0f };
    LayoutProperty<std::string> iconImage { "" };
    LayoutProperty<float> iconRotate { 0.0f };
    LayoutProperty<float> iconPadding { 2.0f };
    LayoutProperty<bool> iconKeepUpright { false };
    LayoutProperty<std::array<float, 2>> iconOffset { {{ 0, 0 }} };

    LayoutProperty<RotationAlignmentType> textRotationAlignment { RotationAlignmentType::Viewport };
    LayoutProperty<std::string> textField { "" };
    LayoutProperty<std::string> textFont { "Open Sans Regular, Arial Unicode MS Regular" };
    LayoutProperty<float> textSize { 16.0f };
    LayoutProperty<float> textMaxWidth { 15.0f /* em */ };
    LayoutProperty<float> textLineHeight { 1.2f /* em */ };
    LayoutProperty<float> textLetterSpacing { 0.0f /* em */ };
    LayoutProperty<TextJustifyType> textJustify { TextJustifyType::Center };
    LayoutProperty<TextAnchorType> textAnchor { TextAnchorType::Center };
    LayoutProperty<float> textMaxAngle { 45.0f /* degrees */ };
    LayoutProperty<float> textRotate { 0.0f };
    LayoutProperty<float> textPadding { 2.0f };
    LayoutProperty<bool> textKeepUpright { true };
    LayoutProperty<TextTransformType> textTransform { TextTransformType::None };
    LayoutProperty<std::array<float, 2>> textOffset { {{ 0, 0 }} };
    LayoutProperty<bool> textAllowOverlap { false };
    LayoutProperty<bool> textIgnorePlacement { false };
    LayoutProperty<bool> textOptional { false };

    // Special case.
    float iconMaxSize = 1.0f;
    float textMaxSize = 16.0f;
};

class SymbolPaintProperties {
public:
    PaintProperty<float> iconOpacity { 1.0f };
    PaintProperty<Color> iconColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<Color> iconHaloColor { {{ 0, 0, 0, 0 }} };
    PaintProperty<float> iconHaloWidth { 0.0f };
    PaintProperty<float> iconHaloBlur { 0.0f };
    PaintProperty<std::array<float, 2>> iconTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> iconTranslateAnchor { TranslateAnchorType::Map };

    PaintProperty<float> textOpacity { 1.0f };
    PaintProperty<Color> textColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<Color> textHaloColor { {{ 0, 0, 0, 0 }} };
    PaintProperty<float> textHaloWidth { 0.0f };
    PaintProperty<float> textHaloBlur { 0.0f };
    PaintProperty<std::array<float, 2>> textTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> textTranslateAnchor { TranslateAnchorType::Map };

    // Special case
    float iconSize = 1.0f;
    float textSize = 16.0f;

    bool isVisible() const {
        return (iconOpacity > 0 && (iconColor.value[3] > 0 || iconHaloColor.value[3] > 0) && iconSize > 0)
            || (textOpacity > 0 && (textColor.value[3] > 0 || textHaloColor.value[3] > 0) && textSize > 0);
    }
};

class SymbolLayer : public StyleLayer {
public:
    SymbolLayer() : StyleLayer(Type::Symbol) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override;
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    SymbolLayoutProperties layout;
    SymbolPaintProperties paint;

    SpriteAtlas* spriteAtlas = nullptr;
};

template <>
inline bool StyleLayer::is<SymbolLayer>() const {
    return type == Type::Symbol;
}

} // namespace mbgl

#endif
