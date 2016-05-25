#pragma once

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class BackgroundPaintProperties {
public:
    PaintProperty<float> backgroundOpacity { 1.0f };
    PaintProperty<Color> backgroundColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::string, CrossFadedFunctionEvaluator> backgroundPattern { "" };
};

class BackgroundLayer : public StyleLayer {
public:
    BackgroundLayer() : StyleLayer(Type::Background) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    BackgroundPaintProperties paint;
};

template <>
inline bool StyleLayer::is<BackgroundLayer>() const {
    return type == Type::Background;
}

} // namespace mbgl
