#ifndef LLMR_RENDERER_FILLBUCKET
#define LLMR_RENDERER_FILLBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/fill_buffer.hpp>

#include <clipper/clipper.hpp>
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
class PatternShader;
struct pbf;

class FillBucket : public Bucket {

    static void *alloc(void *data, unsigned int size);
    static void *realloc(void *data, void *ptr, unsigned int size);
    static void free(void *userData, void *ptr);


    typedef ElementGroup triangle_group_type;
    typedef ElementGroup line_group_type;
public:
    FillBucket(FillVertexBuffer& vertexBuffer,
               TriangleElementsBuffer& triangleElementsBuffer,
               LineElementsBuffer& lineElementsBuffer,
               const BucketFillDescription& properties);
    ~FillBucket();

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);
    virtual bool hasData() const;

    void addGeometry(pbf& data);
    void tessellate();

    void drawElements(PlainShader& shader);
    void drawElements(PatternShader& shader);
    void drawVertices(OutlineShader& shader);

public:
    const BucketFillDescription &properties;

private:
    TESSalloc *allocator;
    TESStesselator *tesselator;
    ClipperLib::Clipper clipper;

    FillVertexBuffer& vertexBuffer;
    TriangleElementsBuffer& triangleElementsBuffer;
    LineElementsBuffer& lineElementsBuffer;

    // hold information on where the vertices are located in the FillBuffer
    const size_t vertex_start;
    const size_t triangle_elements_start;
    const size_t line_elements_start;
    VertexArrayObject array;

    std::vector<triangle_group_type> triangleGroups;
    std::vector<line_group_type> lineGroups;

    std::vector<ClipperLib::IntPoint> line;
    bool hasVertices = false;

    static const int vertexSize = 2;
    static const int stride = sizeof(TESSreal) * vertexSize;
    static const int vertices_per_group = 3;
};

}

#endif
