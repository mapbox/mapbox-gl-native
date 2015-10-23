#ifndef MBGL_FILL_LAYER
#define MBGL_FILL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class FillLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(const StyleCalculationParameters&) override;

    FillPaintProperties properties;
};

}

#endif
