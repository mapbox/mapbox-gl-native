#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

CustomLayer::CustomLayer(const std::string& layerID,
                         CustomLayerInitializeFunction init,
                         CustomLayerRenderFunction render,
                         CustomLayerDeinitializeFunction deinit,
                         void* context)
    : Layer(LayerType::Custom, std::make_unique<Impl>(layerID, init, render, deinit, context))
    , impl(static_cast<Impl*>(baseImpl.get())) {
    Log::Info(Event::General, "New custom layer: %s", layerID.c_str());
}

CustomLayer::CustomLayer(const Impl& other)
    : Layer(LayerType::Custom, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

CustomLayer::~CustomLayer() = default;

template <>
bool Layer::is<CustomLayer>() const {
    return type == LayerType::Custom;
}

} // namespace style
} // namespace mbgl
