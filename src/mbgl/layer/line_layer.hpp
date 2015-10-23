#ifndef MBGL_LINE_LAYER
#define MBGL_LINE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class LineLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(const StyleCalculationParameters&) override;

    LinePaintProperties properties;
};

}

#endif
