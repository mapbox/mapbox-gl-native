#include <mbgl/layermanager/fill_extrusion_layer_factory.hpp>

#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* FillExtrusionLayerFactory::getTypeInfo() const noexcept {
    return style::FillExtrusionLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> FillExtrusionLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }

    std::unique_ptr<style::Layer> layer = std::unique_ptr<style::Layer>(new style::FillExtrusionLayer(id, *source));
    if (!initSourceLayerAndFilter(layer.get(), value)) {
        return nullptr;
    }
    return layer;
}

std::unique_ptr<Layout> FillExtrusionLayerFactory::createLayout(const LayoutParameters& parameters,
                                                                std::unique_ptr<GeometryTileLayer> layer,
                                                                const std::vector<Immutable<style::LayerProperties>>& group) noexcept {
    using namespace style;
    using LayoutType = PatternLayout<FillExtrusionBucket, FillExtrusionLayerProperties, FillExtrusionPattern>;
    return std::make_unique<LayoutType>(parameters.bucketParameters, group, std::move(layer), parameters);
}

std::unique_ptr<RenderLayer> FillExtrusionLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderFillExtrusionLayer>(staticImmutableCast<style::FillExtrusionLayer::Impl>(impl));
}

} // namespace mbgl
