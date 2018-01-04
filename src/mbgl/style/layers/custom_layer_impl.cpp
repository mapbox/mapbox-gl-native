#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::Impl(const std::string& id_,
                         CustomLayerInitializeFunction initializeFn_,
                         CustomLayerRenderFunction renderFn_,
                         CustomLayerContextLostFunction contextLostFn_,
                         CustomLayerDeinitializeFunction deinitializeFn_,
                         void* context_)
    : Layer::Impl(LayerType::Custom, id_, std::string()) {
    initializeFn = initializeFn_;
    renderFn = renderFn_;
    deinitializeFn = deinitializeFn_;
    contextLostFn = contextLostFn_;
    context = context_;
}

bool CustomLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

} // namespace style
} // namespace mbgl
