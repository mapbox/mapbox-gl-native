#ifndef LLMR_RENDERER_LINEBUCKET
#define LLMR_RENDERER_LINEBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <memory>
#include <vector>

namespace llmr {

class Style;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LinejoinShader;
struct pbf;

class LineBucket : public Bucket {
    typedef ElementGroup triangle_group_type;
    typedef ElementGroup point_group_type;
public:
    LineBucket(LineVertexBuffer& vertexBuffer,
               TriangleElementsBuffer& triangleElementsBuffer,
               PointElementsBuffer& pointElementsBuffer,
               const BucketLineDescription& properties);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);
    virtual bool hasData() const;

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);

    bool hasPoints() const;

    void drawLines(LineShader& shader);
    void drawPoints(LinejoinShader& shader);

public:
    const BucketLineDescription &properties;

private:

    LineVertexBuffer& vertexBuffer;
    TriangleElementsBuffer& triangleElementsBuffer;
    PointElementsBuffer& pointElementsBuffer;

    const size_t vertex_start;
    const size_t triangle_elements_start;
    const size_t point_elements_start;

    std::vector<triangle_group_type> triangleGroups;
    std::vector<point_group_type> pointGroups;
};

}

#endif
