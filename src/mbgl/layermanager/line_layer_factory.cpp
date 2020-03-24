#include <mbgl/layermanager/line_layer_factory.hpp>

#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>

namespace mbgl {

const style::LayerTypeInfo* LineLayerFactory::getTypeInfo() const noexcept {
    return style::LineLayer::Impl::staticTypeInfo();
}

std::unique_ptr<style::Layer> LineLayerFactory::createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept {
    optional<std::string> source = getSource(value);
    if (!source) {
        return nullptr;
    }
    return std::unique_ptr<style::Layer>(new style::LineLayer(id, *source));
}

std::unique_ptr<Layout> LineLayerFactory::createLayout(const LayoutParameters& parameters,
                                                       std::unique_ptr<GeometryTileLayer> layer,
                                                       const std::vector<Immutable<style::LayerProperties>>& group) noexcept {
    using namespace style;
    using LayoutTypeUnsorted = PatternLayout<LineBucket, LineLayerProperties, LinePattern, LineLayoutProperties>;
    using LayoutTypeSorted =
        PatternLayout<LineBucket, LineLayerProperties, LinePattern, LineLayoutProperties, LineSortKey>;
    auto layerProperties = staticImmutableCast<LineLayerProperties>(group.front());
    if (layerProperties->layerImpl().layout.get<LineSortKey>().isUndefined()) {
        return std::make_unique<LayoutTypeUnsorted>(parameters.bucketParameters, group, std::move(layer), parameters);
    }
    return std::make_unique<LayoutTypeSorted>(parameters.bucketParameters, group, std::move(layer), parameters);
}

std::unique_ptr<RenderLayer> LineLayerFactory::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderLineLayer>(staticImmutableCast<style::LineLayer::Impl>(impl));
}

} // namespace mbgl
