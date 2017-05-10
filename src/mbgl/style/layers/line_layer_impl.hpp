#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {
namespace style {

class LineLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    LineLayoutProperties::Unevaluated layout;
    LinePaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
