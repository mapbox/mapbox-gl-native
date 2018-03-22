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
         CustomLayerInitializeFunction,
         CustomLayerRenderFunction,
         CustomLayerContextLostFunction,
         CustomLayerDeinitializeFunction,

         CustomLayerContextOwnerChangedFunction,
         CustomLayerContextAttachFunction,
         CustomLayerContextDetachFunction,
         void* context);

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    CustomLayerInitializeFunction initializeFn = nullptr;
    CustomLayerRenderFunction renderFn = nullptr;
    CustomLayerContextLostFunction contextLostFn = nullptr;
    CustomLayerDeinitializeFunction deinitializeFn = nullptr;

    CustomLayerContextOwnerChangedFunction changeOwnerFn = nullptr;
    CustomLayerContextAttachFunction attachFn = nullptr;
    CustomLayerContextDetachFunction detachFn = nullptr;

    void* context = nullptr;

    void didSetObserver(bool didSet) const;
};

} // namespace style
} // namespace mbgl
