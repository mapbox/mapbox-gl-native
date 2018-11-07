#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {
namespace style {

class SymbolLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;
    LayerFactory* getLayerFactory() const noexcept final;

    SymbolLayoutProperties::Unevaluated layout;
    SymbolPaintProperties::Transitionable paint;
};

} // namespace style
} // namespace mbgl
