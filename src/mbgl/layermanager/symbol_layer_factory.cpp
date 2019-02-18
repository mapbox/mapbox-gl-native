#include <mbgl/layermanager/symbol_layer_factory.hpp>

#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* SymbolLayerFactory::getTypeInfo() const noexcept {
    return style::SymbolLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> SymbolLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::SymbolLayer(id, *source));
    if (!initSourceLayerAndFilter(layer.get(), value)) {
        return nullptr;
    }
    return layer;
}

std::unique_ptr<RenderLayer> SymbolLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderSymbolLayer>(staticImmutableCast<style::SymbolLayer::Impl>(std::move(impl)));
}

} // namespace mbgl
