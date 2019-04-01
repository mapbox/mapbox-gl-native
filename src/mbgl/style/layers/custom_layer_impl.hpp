#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

#include <memory>

namespace mbgl {

class TransformState;

namespace style {

class CustomLayer::Impl : public Layer::Impl {
public:
    Impl(const std::string& id,
         std::unique_ptr<CustomLayerHost> host);

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    std::shared_ptr<CustomLayerHost> host;

    DECLARE_LAYER_TYPE_INFO;
};

class CustomLayerProperties final : public LayerProperties {
public:
    explicit CustomLayerProperties(Immutable<CustomLayer::Impl> impl)
        : LayerProperties(std::move(impl)) {}
};

} // namespace style
} // namespace mbgl
