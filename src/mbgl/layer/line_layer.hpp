#ifndef MBGL_LINE_LAYER
#define MBGL_LINE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class LineLayoutProperties {
public:
    LayoutProperty<LineCapType> lineCap { LineCapType::Butt };
    LayoutProperty<LineJoinType> lineJoin { LineJoinType::Miter };
    LayoutProperty<float> lineMiterLimit { 2.0f };
    LayoutProperty<float> lineRoundLimit { 1.0f };
};

class LinePaintProperties {
public:
    PaintProperty<float> lineOpacity { 1.0f };
    PaintProperty<Color> lineColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::array<float, 2>> lineTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> lineTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> lineWidth { 1 };
    PaintProperty<float> lineGapWidth { 0 };
    PaintProperty<float> lineBlur { 0 };
    PaintProperty<float> lineOffset { 0 };
    PaintProperty<std::vector<float>, CrossFadedFunctionEvaluator> lineDasharray { {} };
    PaintProperty<std::string, CrossFadedFunctionEvaluator> linePattern { "" };
};

class LineLayer : public StyleLayer {
public:
    LineLayer() : StyleLayer(Type::Line) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override;
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    LineLayoutProperties layout;
    LinePaintProperties paint;

    float dashLineWidth = 1;
};

template <>
inline bool StyleLayer::is<LineLayer>() const {
    return type == Type::Line;
}

} // namespace mbgl

#endif
