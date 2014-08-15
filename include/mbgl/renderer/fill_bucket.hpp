#ifndef MBGL_RENDERER_FILLBUCKET
#define MBGL_RENDERER_FILLBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/fill_buffer.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <clipper/clipper.hpp>
#include <libtess2/tesselator.h>

#include <vector>
#include <memory>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

namespace mbgl {

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

    typedef ElementGroup<2> triangle_group_type;
    typedef ElementGroup<1> line_group_type;
public:
    FillBucket(FillVertexBuffer& vertexBuffer,
               TriangleElementsBuffer& triangleElementsBuffer,
               LineElementsBuffer& lineElementsBuffer,
               const StyleBucketFill& properties);
    ~FillBucket();

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    void addGeometry(pbf& data);
    void tessellate();

    void drawElements(PlainShader& shader);
    void drawElements(PatternShader& shader);
    void drawVertices(OutlineShader& shader);

public:
    const StyleBucketFill &properties;

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
