#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {
namespace style {

class FillExtrusionLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    FillExtrusionPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
