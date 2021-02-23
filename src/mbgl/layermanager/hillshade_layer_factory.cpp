#include <mbgl/layermanager/hillshade_layer_factory.hpp>

#include <mbgl/renderer/layers/render_hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* HillshadeLayerFactory::getTypeInfo() const noexcept {
    return style::HillshadeLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> HillshadeLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::HillshadeLayer(id, *source));
    return layer;
}

std::unique_ptr<RenderLayer> HillshadeLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderHillshadeLayer>(staticImmutableCast<style::HillshadeLayer::Impl>(impl));
}

} // namespace mbgl
