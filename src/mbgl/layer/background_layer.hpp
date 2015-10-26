#ifndef MBGL_BACKGROUND_LAYER
#define MBGL_BACKGROUND_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class BackgroundPaintProperties {
public:
    PaintProperty<float> opacity = 1.0f;
    PaintProperty<Color> color = { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::string, Faded<std::string>> pattern = { "" };
};

class BackgroundLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override {};
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    BackgroundPaintProperties paint;
};

}

#endif
