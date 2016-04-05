#ifndef MBGL_FILL_LAYER
#define MBGL_FILL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class FillPaintProperties {
public:
    PaintProperty<bool> fillAntialias { true };
    PaintProperty<float> fillOpacity { 1.0f };
    PaintProperty<Color> fillColor { {{ 0, 0, 0, 1 }} };
    PaintProperty<Color> fillOutlineColor { {{ 0, 0, 0, -1 }} };
    PaintProperty<std::array<float, 2>> fillTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> fillTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<std::string, CrossFadedFunctionEvaluator> fillPattern { "" };
};

class FillLayer : public StyleLayer {
public:
    FillLayer() : StyleLayer(Type::Fill) {}
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(
            const GeometryCollection& queryGeometry,
            const GeometryCollection& geometry,
            const float bearing,                
            const float pixelsToTileUnits) const override; 

    FillPaintProperties paint;
};

template <>
inline bool StyleLayer::is<FillLayer>() const {
    return type == Type::Fill;
}

} // namespace mbgl

#endif
