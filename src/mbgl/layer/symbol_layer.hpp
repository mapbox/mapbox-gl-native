#ifndef MBGL_SYMBOL_LAYER
#define MBGL_SYMBOL_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class SymbolLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(const StyleCalculationParameters&) override;

    SymbolPaintProperties properties;
};

}

#endif
