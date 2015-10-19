#ifndef MBGL_RASTER_LAYER
#define MBGL_RASTER_LAYER

#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

class RasterLayer : public StyleLayer {
public:
    RenderPass applyStyleProperties(float z, const TimePoint& now, const ZoomHistory&) override;

    RasterPaintProperties properties;
};

}

#endif
