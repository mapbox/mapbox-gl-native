#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {
namespace style {

class FillLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    FillPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
