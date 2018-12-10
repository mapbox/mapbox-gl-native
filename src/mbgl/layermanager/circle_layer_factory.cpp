#include <mbgl/layermanager/circle_layer_factory.hpp>

#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* CircleLayerFactory::getTypeInfo() const noexcept {
    return style::CircleLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> CircleLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::CircleLayer(id, *source));
    if (!initSourceLayerAndFilter(layer.get(), value)) {
        return nullptr;
    }
    return layer;
}

std::unique_ptr<RenderLayer> CircleLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderCircleLayer>(staticImmutableCast<style::CircleLayer::Impl>(std::move(impl)));
}

} // namespace mbgl
