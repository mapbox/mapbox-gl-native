#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_properties.hpp>

namespace mbgl {
namespace style {

class HillshadeLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    HillshadePaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
