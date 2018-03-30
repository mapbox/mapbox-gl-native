#pragma once

#include <mbgl/style/layer_impl.hpp>
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
};

} // namespace style
} // namespace mbgl
