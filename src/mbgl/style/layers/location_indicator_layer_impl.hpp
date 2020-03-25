#pragma once

#include <array>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_properties.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/geo.hpp>
#include <memory>
#include <string>

namespace mbgl {

class TransformState;

namespace style {

class LocationIndicatorLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl &) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer> &) const override;

    LocationIndicatorLayoutProperties::Unevaluated layout;
    LocationIndicatorPaintProperties::Transitionable paint;
    DECLARE_LAYER_TYPE_INFO;
};

} // namespace style
} // namespace mbgl
