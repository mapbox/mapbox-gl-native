#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile.hpp>

namespace mbgl {

void StyleBucketParameters::eachFilteredFeature(const Filter& filter,
                                                std::function<void (const GeometryTileFeature&)> function) {
    for (std::size_t i = 0; !cancelled() && i < layer.featureCount(); i++) {
        auto feature = layer.getFeature(i);

        FilterEvaluator<GeometryTileFeatureExtractor> evaluator(*feature);
        if (!Filter::visit(filter, evaluator))
            continue;

        function(*feature);
    }
}

} // namespace mbgl
