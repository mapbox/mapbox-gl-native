#ifndef MBGL_CIRCLE_LAYER
#define MBGL_CIRCLE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/paint_properties_map.hpp>

namespace mbgl {

class CircleLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override {};
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    void recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    bool hasTransitions() const override;

    PaintPropertiesMap paints;

    CirclePaintProperties properties;
};

}

#endif
