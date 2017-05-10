#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {
namespace style {

class RasterLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    RasterPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
