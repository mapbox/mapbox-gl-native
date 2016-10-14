#include <mbgl/style/layers/terrain_layer_impl.hpp>
#include <mbgl/style/geometry_bucket_parameters.hpp>
#include <mbgl/style/raster_bucket_parameters.hpp>
#include <mbgl/renderer/terrain_bucket.hpp>

namespace mbgl {
namespace style {

void TerrainLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool TerrainLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = RenderPass::Translucent;

    return hasTransitions;
}

std::unique_ptr<Bucket> TerrainLayer::Impl::createBucket(BucketParameters& params) const {
    if (params.is<mapbox::util::recursive_wrapper<GeometryBucketParameters>>()) {
        // DEM data encoded into vector tile
        auto& parameters = params.get<GeometryBucketParameters>();
        auto& layer = parameters.layer;

        for (std::size_t i = 0; !parameters.cancelled() && i < layer.featureCount(); i++) {
            auto feature = layer.getFeature(i);
            if (feature->getType() == FeatureType(4)) {
                return std::make_unique<TerrainBucket>(feature->getDEMPyramid());
            } else {
                // Trying to create a DEM from another feature type.
            }
        }
    } else if (params.is<mapbox::util::recursive_wrapper<RasterBucketParameters>>()) {
        // DEM data from raster tile
        auto& parameters = params.get<RasterBucketParameters>();
        if (parameters.data) {
            return std::make_unique<TerrainBucket>(decodeImage(*parameters.data));
        }
    }

    return nullptr;
}

} // namespace style
} // namespace mbgl
