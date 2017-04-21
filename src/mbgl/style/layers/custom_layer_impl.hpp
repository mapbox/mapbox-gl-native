#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/custom_layer.hpp>

namespace mbgl {

class TransformState;

namespace style {

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
    void deinitialize();
    void render(const TransformState&) const;

private:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    std::unique_ptr<RenderLayer> createRenderLayer() const final;

    CustomLayerInitializeFunction initializeFn = nullptr;
    CustomLayerRenderFunction renderFn = nullptr;
    CustomLayerDeinitializeFunction deinitializeFn = nullptr;
    void* context = nullptr;
};

} // namespace style
} // namespace mbgl
