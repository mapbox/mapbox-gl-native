#include <mbgl/layermanager/heatmap_layer_factory.hpp>

#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* HeatmapLayerFactory::getTypeInfo() const noexcept {
    return style::HeatmapLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> HeatmapLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::HeatmapLayer(id, *source));
    if (!initSourceLayerAndFilter(layer.get(), value)) {
        return nullptr;
    }
    return layer;
}

std::unique_ptr<RenderLayer> HeatmapLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderHeatmapLayer>(staticImmutableCast<style::HeatmapLayer::Impl>(std::move(impl)));
}

} // namespace mbgl
