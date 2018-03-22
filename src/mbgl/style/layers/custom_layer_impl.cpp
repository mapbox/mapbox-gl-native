#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::~Impl() {
    if (changeOwnerFn)
        changeOwnerFn(context, NULL);
}

CustomLayer::Impl::Impl(const std::string& id_,
                        CustomLayerInitializeFunction initializeFn_,
                        CustomLayerRenderFunction renderFn_,
                        CustomLayerContextLostFunction contextLostFn_,
                        CustomLayerDeinitializeFunction deinitializeFn_,

                        CustomLayerContextOwnerChangedFunction changeOwnerFn_,
                        CustomLayerContextAttachFunction attachFn_,
                        CustomLayerContextDetachFunction detachFn_,

                        void* context_):
    Layer::Impl(LayerType::Custom, id_, std::string()),
    initializeFn(initializeFn_),
    renderFn(renderFn_),
    contextLostFn(contextLostFn_),
    deinitializeFn(deinitializeFn_),
    changeOwnerFn(changeOwnerFn_),
    attachFn(attachFn_),
    detachFn(detachFn_),
    context(context_) {

        if (changeOwnerFn)
            changeOwnerFn(context, this);
}

void CustomLayer::Impl::didSetObserver(bool didSet) const {
    if (didSet) {
        if (attachFn) attachFn(context);
    }
    else {
        if (detachFn) detachFn(context);
    }
}

bool CustomLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

} // namespace style
} // namespace mbgl
