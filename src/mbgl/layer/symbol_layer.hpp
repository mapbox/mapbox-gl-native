#ifndef MBGL_SYMBOL_LAYER
#define MBGL_SYMBOL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class SymbolLayoutProperties {
public:
    LayoutProperty<PlacementType> placement = PlacementType::Point;
    LayoutProperty<float> spacing = 250.0f;
    LayoutProperty<bool> avoidEdges = false;

    class IconProperties {
    public:
        LayoutProperty<bool> allowOverlap = false;
        LayoutProperty<bool> ignorePlacement = false;
        LayoutProperty<bool> optional = false;
        LayoutProperty<RotationAlignmentType> rotationAlignment = RotationAlignmentType::Viewport;
        LayoutProperty<float> size = 1.0f;
        LayoutProperty<std::string> image = { "" };
        LayoutProperty<float> rotate = 0.0f;
        LayoutProperty<float> padding = 2.0f;
        LayoutProperty<bool> keepUpright = false;
        LayoutProperty<std::array<float, 2>> offset = { {{ 0, 0 }} };
    } icon;

    class TextProperties {
    public:
        LayoutProperty<RotationAlignmentType> rotationAlignment = RotationAlignmentType::Viewport;
        LayoutProperty<std::string> field = { "" };
        LayoutProperty<std::string> font = { "Open Sans Regular, Arial Unicode MS Regular" };
        LayoutProperty<float> size = 16.0f;
        LayoutProperty<float> maxWidth = 15.0f /* em */;
        LayoutProperty<float> lineHeight = 1.2f /* em */;
        LayoutProperty<float> letterSpacing = 0.0f /* em */;
        LayoutProperty<TextJustifyType> justify = TextJustifyType::Center;
        LayoutProperty<TextAnchorType> anchor = TextAnchorType::Center;
        LayoutProperty<float> maxAngle = 45.0f /* degrees */;
        LayoutProperty<float> rotate = 0.0f;
        LayoutProperty<float> padding = 2.0f;
        LayoutProperty<bool> keepUpright = true;
        LayoutProperty<TextTransformType> transform = TextTransformType::None;
        LayoutProperty<std::array<float, 2>> offset = { {{ 0, 0 }} };
        LayoutProperty<bool> allowOverlap = false;
        LayoutProperty<bool> ignorePlacement = false;
        LayoutProperty<bool> optional = false;
    } text;

    // Special case.
    float iconMaxSize = 1.0f;
    float textMaxSize = 16.0f;
};

class SymbolPaintProperties {
public:
    class PaintProperties {
    public:
        PaintProperties(float size_) : size(size_) {}

        PaintProperty<float> opacity = 1.0f;
        PaintProperty<Color> color = { {{ 0, 0, 0, 1 }} };
        PaintProperty<Color> haloColor = { {{ 0, 0, 0, 0 }} };
        PaintProperty<float> haloWidth = 0.0f;
        PaintProperty<float> haloBlur = 0.0f;
        PaintProperty<std::array<float, 2>> translate = { {{ 0, 0 }} };
        PaintProperty<TranslateAnchorType> translateAnchor = TranslateAnchorType::Map;

        // Special case
        float size;

        bool isVisible() const {
            return opacity > 0 && (color.value[3] > 0 || haloColor.value[3] > 0) && size > 0;
        }
    };

    PaintProperties icon { 1.0f };
    PaintProperties text { 16.0f };
};

class SymbolLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override;
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    SymbolLayoutProperties layout;
    SymbolPaintProperties paint;
};

}

#endif
