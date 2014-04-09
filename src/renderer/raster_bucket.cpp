#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/renderer/painter.hpp>

using namespace llmr;

RasterBucket::RasterBucket(const BucketDescription& bucket_desc) {
}

void RasterBucket::render(Painter& painter, const std::string& layer_name, const Tile::ID& id) {
    // The painter renders rasters directly. This function is a no-op
    // and exists just to satisfy the abstract superclass requirements.
    // That way, we can avoid having raster buckets pass around
    // references to tile data and/or rasters directly. 
}
