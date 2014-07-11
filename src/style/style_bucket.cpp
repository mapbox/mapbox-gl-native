#include <llmr/style/style_bucket.hpp>

namespace llmr {

StyleBucket::StyleBucket(StyleLayerType type) {
    switch (type) {
        case StyleLayerType::Fill: render = StyleBucketFill{}; break;
        case StyleLayerType::Line: render = StyleBucketLine{}; break;
        case StyleLayerType::Icon: render = StyleBucketIcon{}; break;
        case StyleLayerType::Text: render = StyleBucketText{}; break;
        case StyleLayerType::Raster: render = StyleBucketRaster{}; break;
        default: break;
    }
}

}