#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

StyleBucket::StyleBucket(StyleLayerType type) {
    switch (type) {
        case StyleLayerType::Fill: render = StyleBucketFill{}; break;
        case StyleLayerType::Line: render = StyleBucketLine{}; break;
        case StyleLayerType::Symbol: render = StyleBucketSymbol{}; break;
        case StyleLayerType::Raster: render = StyleBucketRaster{}; break;
        default: break;
    }
}

}