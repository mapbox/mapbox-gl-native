#ifndef MBGL_CIRCLE_LAYER
#define MBGL_CIRCLE_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class CircleLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(float z, const TimePoint& now, const ZoomHistory&) override;

    CirclePaintProperties properties;
};

}

#endif
