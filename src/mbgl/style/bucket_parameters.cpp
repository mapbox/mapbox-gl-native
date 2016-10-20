#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

namespace mbgl {
namespace style {

void BucketParameters::eachFilteredFeature(const Filter& filter,
                                           const GeometryTileLayer& layer,
                                           std::function<void (const GeometryTileFeature&, std::size_t index, const std::string& layerName)> function) {
    auto name = layer.getName();
    for (std::size_t i = 0; !cancelled() && i < layer.featureCount(); i++) {
        auto feature = layer.getFeature(i);
        if (!filter(feature->getType(), feature->getID(), [&] (const auto& key) { return feature->getValue(key); }))
            continue;
        function(*feature, i, name);
    }
}

} // namespace style
} // namespace mbgl
