#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/fill_buffer.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace llmr {

class Style;
class FillBuffer;
class BucketDescription;
class OutlineShader;
class PlainShader;
struct Coordinate;
struct pbf;

class FillBucket : public Bucket {
public:
    FillBucket(const std::shared_ptr<FillBuffer>& buffer, const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);
    uint32_t size() const;

    void drawElements(PlainShader& shader);
    void drawVertices(OutlineShader& shader);

public:
    const BucketGeometryDescription geom_desc;

private:
    std::shared_ptr<FillBuffer> buffer;

    struct group {
        VertexArrayObject<PlainShader> array;
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
    VertexArrayObject<OutlineShader> array;
    std::vector<group> groups;
};

}

#endif
