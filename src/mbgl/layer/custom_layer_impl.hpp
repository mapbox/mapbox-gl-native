#pragma once

#include <mbgl/layer/layer_impl.hpp>
#include <mbgl/layer/custom_layer.hpp>

namespace mbgl {

class TransformState;

class CustomLayer::Impl : public Layer::Impl {
public:
    Impl(const std::string& id,
         CustomLayerInitializeFunction,
         CustomLayerRenderFunction,
         CustomLayerDeinitializeFunction,
         void* context);

    Impl(const Impl&);
    ~Impl() final;

    void initialize();
    void render(const TransformState&) const;

private:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) final {}
    void parsePaints(const JSValue&) final {}

    void cascade(const StyleCascadeParameters&) final {}
    bool recalculate(const StyleCalculationParameters&) final;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const final;

    CustomLayerInitializeFunction initializeFn = nullptr;
    CustomLayerRenderFunction renderFn = nullptr;
    CustomLayerDeinitializeFunction deinitializeFn = nullptr;
    void* context = nullptr;
};

} // namespace mbgl
