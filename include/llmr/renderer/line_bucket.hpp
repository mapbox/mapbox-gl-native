#ifndef LLMR_RENDERER_LINEBUCKET
#define LLMR_RENDERER_LINEBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <memory>
#include <vector>

namespace llmr {

class Style;
class LineBuffer;
struct Coordinate;
struct pbf;

class LineBucket : public Bucket {
public:
    LineBucket(const std::shared_ptr<LineBuffer>& buffer, const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);
    uint32_t size() const;

    void bind();
    char *vertexOffset() const;

public:
    const BucketGeometryDescription geometry;

private:
    std::shared_ptr<LineBuffer> buffer;
    uint32_t start;
    uint32_t length;
};

}

#endif
