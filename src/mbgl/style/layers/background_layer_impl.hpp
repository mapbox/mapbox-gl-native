#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {
namespace style {

class BackgroundLayer::Impl : public Layer::Impl {
public:
    template <typename... Args>
    Impl(Args&&... args) : Layer::Impl(std::forward<Args>(args)...)
    {}

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    BackgroundPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
