#include <mbgl/layermanager/layer_factory.hpp>

#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>

#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>

namespace mbgl {

optional<std::string> LayerFactory::getSource(const style::conversion::Convertible& value) const noexcept {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        return nullopt;
    }

    return source;
}

std::unique_ptr<Bucket> LayerFactory::createBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&) noexcept {
    assert(false);
    return nullptr;
}

std::unique_ptr<Layout> LayerFactory::createLayout(const LayoutParameters&,
                                                   std::unique_ptr<GeometryTileLayer>,
                                                   const std::vector<Immutable<style::LayerProperties>>&) noexcept {
    assert(false);
    return nullptr;
}

bool LayerFactory::initSourceLayerAndFilter(style::Layer* layer, const style::conversion::Convertible& value) const noexcept {
    auto sourceLayerValue = objectMember(value, "source-layer");
    if (sourceLayerValue) {
        optional<std::string> sourceLayer = toString(*sourceLayerValue);
        if (!sourceLayer) {
            return false;
        }
        layer->setSourceLayer(*sourceLayer);
    }

    auto filterValue = objectMember(value, "filter");
    if (filterValue) {
        style::conversion::Error error;
        optional<style::Filter> filter = style::conversion::convert<style::Filter>(*filterValue, error);
        if (!filter) {
            return false;
        }
        layer->setFilter(*filter);
    }

    return true;
}

} // namespace mbgl
