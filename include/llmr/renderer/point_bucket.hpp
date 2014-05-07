#ifndef LLMR_RENDERER_POINTBUCKET
#define LLMR_RENDERER_POINTBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/point_buffer.hpp>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace llmr {

class Style;
class PointVertexBuffer;
class BucketDescription;
class PointShader;
class DotShader;
struct pbf;

class PointBucket : public Bucket {
public:
    PointBucket(PointVertexBuffer& vertexBuffer,
                const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);

    bool hasPoints() const;

    void drawPoints(PointShader& shader);
    void drawPoints(DotShader& shader);

public:
    const BucketGeometryDescription geometry;

private:

    PointVertexBuffer& vertexBuffer;
    VertexArrayObject array;

    const size_t vertex_start;
    size_t vertex_end = 0;
};

}

#endif
