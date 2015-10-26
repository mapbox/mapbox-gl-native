#ifndef MBGL_FILL_LAYER
#define MBGL_FILL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class FillPaintProperties {
public:
    PaintProperty<bool> antialias = true;
    PaintProperty<float> opacity = 1.0f;
    PaintProperty<Color> color = { {{ 0, 0, 0, 1 }} };
    PaintProperty<Color> outlineColor = { {{ 0, 0, 0, -1 }} };
    PaintProperty<std::array<float, 2>> translate = { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> translateAnchor = TranslateAnchorType::Map;
    PaintProperty<std::string, Faded<std::string>> pattern = { "" };
};

class FillLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override {};
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    FillPaintProperties paint;
};

}

#endif
