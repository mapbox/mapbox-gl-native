#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/map/geometry_tile.hpp>

namespace mbgl {

void StyleBucketParameters::eachFilteredFeature(const FilterExpression& filter,
                                                std::function<void (const GeometryTileFeature&)> function) {
    for (std::size_t i = 0; !cancelled() && i < layer.featureCount(); i++) {
        auto feature = layer.getFeature(i);

        GeometryTileFeatureExtractor extractor(*feature);
        if (!evaluate(filter, extractor))
            continue;

        function(*feature);
    }
}

}
