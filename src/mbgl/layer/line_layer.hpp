#ifndef MBGL_LINE_LAYER
#define MBGL_LINE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/paint_properties_map.hpp>
#include <mbgl/style/layout_property.hpp>

namespace mbgl {

class LineLayoutProperties {
public:
    LayoutProperty<CapType> cap = CapType::Butt;
    LayoutProperty<JoinType> join = JoinType::Miter;
    LayoutProperty<float> miterLimit = 2.0f;
    LayoutProperty<float> roundLimit = 1.0f;
};

class LineLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override;
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    void recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    bool hasTransitions() const override;

    LineLayoutProperties layout;
    PaintPropertiesMap paints;

    LinePaintProperties properties;
};

}

#endif
