#ifndef LLMR_RENDERER_RASTERBUCKET
#define LLMR_RENDERER_RASTERBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>

namespace llmr {

class BucketDescription;

class RasterBucket : public Bucket {
public:
    RasterBucket(const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);
};

}

#endif
