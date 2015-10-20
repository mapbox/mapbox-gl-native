#ifndef MBGL_LINE_LAYER
#define MBGL_LINE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/paint_properties_map.hpp>
#include <mbgl/style/class_properties.hpp>

namespace mbgl {

class LineLayer : public StyleLayer {
public:
    void parseLayout(const JSVal&) override;
    void parsePaints(const JSVal&) override;

    void recalculate(const StyleCalculationParameters&) override;

    LinePaintProperties properties;
};

}

#endif
