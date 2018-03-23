#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

namespace mbgl {

class TransformState;

namespace style {

class CustomLayer::Impl : public Layer::Impl {
public:
    virtual ~Impl();

    Impl(const std::string& id,
         std::unique_ptr<CustomLayerContext> context);

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    std::shared_ptr<CustomLayerContext> context;

    void didSetObserver(bool didSet) const;
};

} // namespace style
} // namespace mbgl
