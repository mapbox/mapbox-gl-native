#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {
namespace style {

class HeatmapLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;
    LayerFactory* getLayerFactory() const final;

    HeatmapPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
