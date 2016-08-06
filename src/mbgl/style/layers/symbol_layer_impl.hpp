#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

class SpriteAtlas;

namespace style {

class SymbolLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;

    void cascade(const CascadeParameters&) override;
    bool recalculate(const CalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&) const override;

    SymbolLayoutProperties layout;
    SymbolPaintProperties paint;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    SpriteAtlas* spriteAtlas = nullptr;
};

} // namespace style
} // namespace mbgl
