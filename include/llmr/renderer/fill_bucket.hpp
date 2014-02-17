#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/fill_buffer.hpp>

#include <libtess2/tesselator.h>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace llmr {

class Style;
class FillVertexBuffer;
class TriangleElementsBuffer;
class LineElementsBuffer;
class BucketDescription;
class OutlineShader;
class PlainShader;
struct Coordinate;
struct pbf;

class FillBucket : public Bucket {

    static void *alloc(void *data, unsigned int size);
    static void *realloc(void *data, void *ptr, unsigned int size);
    static void free(void *userData, void *ptr);


    typedef ElementGroup<PlainShader> triangle_group_type;
    typedef ElementGroup<OutlineShader> line_group_type;
public:
    FillBucket(const std::shared_ptr<FillVertexBuffer>& vertexBuffer,
               const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
               const std::shared_ptr<LineElementsBuffer>& lineElementsBuffer,
               const BucketDescription& bucket_desc);
    ~FillBucket();

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);
    void addGeometry(const std::vector<std::vector<TESSreal>>& lines);
    bool empty() const;
    void tessellate();

    void drawElements(PlainShader& shader);
    void drawVertices(OutlineShader& shader);

public:
    const BucketGeometryDescription geom_desc;

private:
    TESSalloc *allocator;
    TESStesselator *tesselator;

    std::shared_ptr<FillVertexBuffer> vertexBuffer;
    std::shared_ptr<TriangleElementsBuffer> triangleElementsBuffer;
    std::shared_ptr<LineElementsBuffer> lineElementsBuffer;

    // hold information on where the vertices are located in the FillBuffer
    const uint32_t vertex_start;
    const uint32_t triangle_elements_start;
    const uint32_t line_elements_start;
    VertexArrayObject<OutlineShader> array;
    std::vector<triangle_group_type> triangleGroups;
    std::vector<line_group_type> lineGroups;

    std::vector<TESSreal> line;
    bool hasVertices = false;

    static const int vertexSize = 2;
    static const int stride = sizeof(TESSreal) *vertexSize;
    static const int vertices_per_group = 3;
};

}

#endif
