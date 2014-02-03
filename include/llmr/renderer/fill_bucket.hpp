#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>

#include <vector>
#include <memory>

namespace llmr {

class Style;
class FillBuffer;
class BucketDescription;
struct Coordinate;
struct pbf;

class FillBucket : public Bucket {
public:
    FillBucket(const std::shared_ptr<FillBuffer>& buffer, const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);
    uint32_t size() const;
    void drawElements(int32_t attrib);
    void drawVertices(int32_t attrib);

public:
    const BucketGeometryDescription geom_desc;

private:
    std::shared_ptr<FillBuffer> buffer;

    struct group {
        uint32_t vertex_length;
        uint32_t elements_length;

        group() : vertex_length(0), elements_length(0) {}
        group(uint32_t vertex_length, uint32_t elements_length)
            : vertex_length(vertex_length),
              elements_length(elements_length) {
        }
    };

    // hold information on where the vertices are located in the FillBuffer
    uint32_t vertex_start;
    uint32_t elements_start;
    uint32_t length;
    std::vector<group> groups;
};

}

#endif
