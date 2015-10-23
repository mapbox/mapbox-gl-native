#ifndef MBGL_BACKGROUND_LAYER
#define MBGL_BACKGROUND_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class BackgroundLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(const StyleCalculationParameters&) override;

    BackgroundPaintProperties properties;
};

}

#endif
