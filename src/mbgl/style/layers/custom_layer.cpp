#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

CustomLayer::CustomLayer(const std::string& layerID,
                         CustomLayerInitializeFunction init,
                         CustomLayerRenderFunction render,
                         CustomLayerContextLostFunction contextLost,
                         CustomLayerDeinitializeFunction deinit,
                         void* context)
        : Layer(makeMutable<Impl>(layerID, init, render, contextLost, deinit, context)) {
}

CustomLayer::CustomLayer(const std::string& layerID,
                         CustomLayerInitializeFunction init,
                         CustomLayerRenderFunction render,
                         CustomLayerDeinitializeFunction deinit,
                         void* context)
    : Layer(makeMutable<Impl>(layerID, init, render, nullptr, deinit, context)) {
}

CustomLayer::~CustomLayer() = default;

const CustomLayer::Impl& CustomLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CustomLayer::Impl> CustomLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CustomLayer::cloneRef(const std::string&) const {
    assert(false);
    return nullptr;
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
