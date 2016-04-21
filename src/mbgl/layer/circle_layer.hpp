#ifndef MBGL_CIRCLE_LAYER
#define MBGL_CIRCLE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class CirclePaintProperties {
public:
    PaintProperty<float> circleRadius { 5.0f };
    PaintProperty<Color> circleColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<float> circleOpacity { 1.0f };
    PaintProperty<std::array<float, 2>> circleTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> circleTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> circleBlur { 0 };

    bool isVisible() const {
        return circleRadius > 0 && circleColor.value[3] > 0 && circleOpacity > 0;
    }
};

class CircleLayer : public StyleLayer {
public:
    CircleLayer() : StyleLayer(Type::Circle) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    CirclePaintProperties paint;
};

template <>
inline bool StyleLayer::is<CircleLayer>() const {
    return type == Type::Circle;
}

} // namespace mbgl

#endif
