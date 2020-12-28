#include <mbgl/layermanager/location_indicator_layer_factory.hpp>

#include <mbgl/renderer/layers/render_location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* LocationIndicatorLayerFactory::getTypeInfo() const noexcept {
    return style::LocationIndicatorLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> LocationIndicatorLayerFactory::createLayer(
    const std::string& id, const style::conversion::Convertible&) noexcept {
    return std::unique_ptr<style::Layer>(new style::LocationIndicatorLayer(id));
}

std::unique_ptr<RenderLayer> LocationIndicatorLayerFactory::createRenderLayer(
    Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderLocationIndicatorLayer>(
        staticImmutableCast<style::LocationIndicatorLayer::Impl>(impl));
}

} // namespace mbgl
