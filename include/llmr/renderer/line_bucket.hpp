#ifndef LLMR_RENDERER_LINEBUCKET
#define LLMR_RENDERER_LINEBUCKET

#include "bucket.hpp"

#include <vector>

namespace llmr {

class Style;
class LineBuffer;
class BucketDescription;
struct Coordinate;
struct pbf;

class LineBucket : public Bucket {
public:
    LineBucket(const std::shared_ptr<LineBuffer>& buffer);

    virtual void render(Painter& painter, const std::string& layer_name);

    void addGeometry(pbf& data, const BucketDescription& bucket_desc);
    void addGeometry(const std::vector<Coordinate>& line, const BucketDescription& bucket_desc);

private:
    std::shared_ptr<LineBuffer> buffer;
    uint32_t start;
    uint32_t length;
};

}

#endif
