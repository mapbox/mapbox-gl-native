#ifndef MBGL_CIRCLE_LAYER
#define MBGL_CIRCLE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class CirclePaintProperties {
public:
    PaintProperty<float> radius = 5.0f;
    PaintProperty<Color> color = { {{ 0, 0, 0, 1 }} };
    PaintProperty<float> opacity = 1.0f;
    PaintProperty<std::array<float, 2>> translate = { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> translateAnchor = TranslateAnchorType::Map;
    PaintProperty<float> blur = 0;

    bool isVisible() const {
        return radius > 0 && color.value[3] > 0 && opacity > 0;
    }
};

class CircleLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override {};
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    CirclePaintProperties paint;
};

}

#endif
