#include <mbgl/layermanager/raster_layer_factory.hpp>

#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* RasterLayerFactory::getTypeInfo() const noexcept {
    return style::RasterLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> RasterLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::RasterLayer(id, *source));
    return layer;
}

std::unique_ptr<RenderLayer> RasterLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderRasterLayer>(staticImmutableCast<style::RasterLayer::Impl>(impl));
}

} // namespace mbgl
