#ifndef MBGL_RENDERER_FILLBUCKET
#define MBGL_RENDERER_FILLBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/elements_buffer.hpp>

#include <clipper/clipper.hpp>
#include <libtess2/tesselator.h>

#include <vector>
#include <memory>

namespace mbgl {

class FillVertexBuffer;
class OutlineShader;
class PlainShader;
class PatternShader;

class FillBucket : public Bucket {

    static void *alloc(void *data, unsigned int size);
    static void *realloc(void *data, void *ptr, unsigned int size);
    static void free(void *userData, void *ptr);

    typedef ElementGroup<2> TriangleGroup;
    typedef ElementGroup<1> LineGroup;

public:
    FillBucket(FillVertexBuffer &vertexBuffer,
               TriangleElementsBuffer &triangleElementsBuffer,
               LineElementsBuffer &lineElementsBuffer);
    ~FillBucket() override;

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;
    bool hasData() const;

    void addGeometry(const GeometryCollection&);
    void tessellate();

    void drawElements(PlainShader& shader);
    void drawElements(PatternShader& shader);
    void drawVertices(OutlineShader& shader);

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

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
    std::vector<std::unique_ptr<LineGroup>> lineGroups;

    std::vector<ClipperLib::IntPoint> line;
    bool hasVertices = false;

    static const int vertexSize = 2;
    static const int stride = sizeof(TESSreal) * vertexSize;
    static const int vertices_per_group = 3;
};

}

#endif
