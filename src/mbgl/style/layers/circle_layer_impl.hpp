#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {
namespace style {

class CircleLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    std::unique_ptr<RenderLayer> createRenderLayer() const override;

    CirclePaintProperties::Cascading cascading;
};

} // namespace style
} // namespace mbgl
