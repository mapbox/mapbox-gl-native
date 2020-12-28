#include <mbgl/gl/custom_layer.hpp>
#include <mbgl/gl/custom_layer_factory.hpp>
#include <mbgl/gl/custom_layer_impl.hpp>
#include <mbgl/gl/render_custom_layer.hpp>

namespace mbgl {

const style::LayerTypeInfo* CustomLayerFactory::getTypeInfo() const noexcept {
    return style::CustomLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> CustomLayerFactory::createLayer(const std::string&, const style::conversion::Convertible&) noexcept {
    assert(false);
    return nullptr;
}

std::unique_ptr<RenderLayer> CustomLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    return std::make_unique<RenderCustomLayer>(staticImmutableCast<style::CustomLayer::Impl>(impl));
}

} // namespace mbgl
