#pragma once

#include <mbgl/layer/layer_impl.hpp>
#include <mbgl/layer/background_layer.hpp>
#include <mbgl/layer/background_layer_properties.hpp>

namespace mbgl {

class BackgroundLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    BackgroundPaintProperties paint;
};

} // namespace mbgl
