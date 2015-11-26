#ifndef MBGL_RASTER_LAYER
#define MBGL_RASTER_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class RasterPaintProperties {
public:
    PaintProperty<float> opacity = 1.0f;
    PaintProperty<float> hueRotate = 0.0f;
    PaintProperty<float> brightnessMin = 0.0f;
    PaintProperty<float> brightnessMax = 1.0f;
    PaintProperty<float> saturation = 0.0f;
    PaintProperty<float> contrast = 0.0f;
    PaintProperty<float> fadeDuration = 0.0f;
};

class RasterLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override {};
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    RasterPaintProperties paint;
};

}

#endif
