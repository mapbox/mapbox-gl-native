#include <mbgl/layermanager/circle_layer_factory.hpp>

#include <mbgl/layout/circle_layout.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
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

    return std::unique_ptr<style::Layer>(new style::CircleLayer(id, *source));
}

std::unique_ptr<Layout> CircleLayerFactory::createLayout(
    const LayoutParameters& parameters,
    std::unique_ptr<GeometryTileLayer> layer,
    const std::vector<Immutable<style::LayerProperties>>& group) noexcept {
    return std::make_unique<CircleLayout>(parameters.bucketParameters, group, std::move(layer));
}

std::unique_ptr<RenderLayer> CircleLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderCircleLayer>(staticImmutableCast<style::CircleLayer::Impl>(impl));
}

} // namespace mbgl
