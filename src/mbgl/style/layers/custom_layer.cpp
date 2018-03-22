#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

CustomLayer::~CustomLayer()
{
    printf("~CustomLayer destructor %p\n", this);

}

CustomLayer::CustomLayer(const std::string& layerID,
                         CustomLayerInitializeFunction init,
                         CustomLayerRenderFunction render,
                         CustomLayerContextLostFunction contextLost,
                         CustomLayerDeinitializeFunction deinit,

                         CustomLayerContextOwnerChangedFunction ownerChanged,
                         CustomLayerContextAttachFunction attach,
                         CustomLayerContextDetachFunction detach,
                         void* context)
    : Layer(makeMutable<Impl>(layerID, init, render, contextLost, deinit, ownerChanged, attach, detach, context))
{
    printf("CustomLayer::CustomLayer %p\n", this);

}

CustomLayer::CustomLayer(const std::string& layerID,
                         CustomLayerInitializeFunction init,
                         CustomLayerRenderFunction render,
                         CustomLayerDeinitializeFunction deinit,

                         CustomLayerContextOwnerChangedFunction ownerChanged,
                         CustomLayerContextAttachFunction attach,
                         CustomLayerContextDetachFunction detach,
                         void* context)
    : Layer(makeMutable<Impl>(layerID, init, render, nullptr, deinit, ownerChanged, attach, detach, context))
{
    printf("CustomLayer::CustomLayer %p\n", this);
}

const CustomLayer::Impl& CustomLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

const void* CustomLayer::getContext() const {
    return impl().context;
}

Mutable<CustomLayer::Impl> CustomLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CustomLayer::cloneRef(const std::string&) const {
    assert(false);
    return nullptr;
}

void CustomLayer::setObserver(LayerObserver* observer) {

    Layer::setObserver(observer);

    impl().didSetObserver(observer != NULL);
}

// Visibility

void CustomLayer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Zoom range

void CustomLayer::setMinZoom(float minZoom) {
    auto impl_ = mutableImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
}

void CustomLayer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
}

template <>
bool Layer::is<CustomLayer>() const {
    return getType() == LayerType::Custom;
}

} // namespace style
} // namespace mbgl
