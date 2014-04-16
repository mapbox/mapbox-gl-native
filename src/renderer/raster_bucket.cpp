#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/renderer/painter.hpp>

using namespace llmr;

RasterBucket::RasterBucket(const BucketDescription &) {
}

void RasterBucket::render(Painter &, const std::string &, const Tile::ID &) {
    // The painter renders rasters directly. This function is a no-op
    // and exists just to satisfy the abstract superclass requirements.
    // That way, we can avoid having raster buckets pass around
    // references to tile data and/or rasters directly. 
}
