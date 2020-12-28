#include <mbgl/layermanager/background_layer_factory.hpp>

#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* BackgroundLayerFactory::getTypeInfo() const noexcept {
    return style::BackgroundLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> BackgroundLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    (void)value;
    return std::unique_ptr<style::Layer>(new style::BackgroundLayer(id));
}

std::unique_ptr<RenderLayer> BackgroundLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderBackgroundLayer>(staticImmutableCast<style::BackgroundLayer::Impl>(impl));
}

} // namespace mbgl
