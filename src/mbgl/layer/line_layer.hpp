#ifndef MBGL_LINE_LAYER
#define MBGL_LINE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {

class LineLayoutProperties {
public:
    LayoutProperty<CapType> cap = CapType::Butt;
    LayoutProperty<JoinType> join = JoinType::Miter;
    LayoutProperty<float> miterLimit = 2.0f;
    LayoutProperty<float> roundLimit = 1.0f;
};

class LinePaintProperties {
public:
    PaintProperty<float> opacity = 1.0f;
    PaintProperty<Color> color = { {{ 0, 0, 0, 1 }} };
    PaintProperty<std::array<float, 2>> translate = { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> translateAnchor = TranslateAnchorType::Map;
    PaintProperty<float> width = 1;
    PaintProperty<float> gapWidth = 0;
    PaintProperty<float> blur = 0;
    PaintProperty<std::vector<float>, Faded<std::vector<float>>> dasharray = { {} };
    PaintProperty<std::string, Faded<std::string>> pattern = { "" };

    // Special case
    float dashLineWidth = 1;

    bool isVisible() const {
        return opacity > 0 && color.value[3] > 0 && width > 0;
    }
};

class LineLayer : public StyleLayer {
public:
    std::unique_ptr<StyleLayer> clone() const override;

    void parseLayout(const JSVal&) override;
    void parsePaints(const JSVal&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    LineLayoutProperties layout;
    LinePaintProperties paint;
};

}

#endif
