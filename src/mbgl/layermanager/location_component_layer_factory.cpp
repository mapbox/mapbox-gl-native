#include <mbgl/layermanager/location_component_layer_factory.hpp>
#include <mbgl/renderer/layers/render_location_component_layer.hpp>
#include <mbgl/style/layers/location_component_layer.hpp>
#include <mbgl/style/layers/location_component_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* LocationComponentLayerFactory::getTypeInfo() const noexcept {
    return style::LocationComponentLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> LocationComponentLayerFactory::createLayer(
    const std::string& id, const style::conversion::Convertible& value) noexcept {
    (void)value;
    return std::unique_ptr<style::Layer>(new style::LocationComponentLayer(id)); // Need the map here. Or as a property?
}

std::unique_ptr<RenderLayer> LocationComponentLayerFactory::createRenderLayer(
    Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderLocationComponentLayer>(
        staticImmutableCast<style::LocationComponentLayer::Impl>(impl));
}

} // namespace mbgl
