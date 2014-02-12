#ifndef LLMR_RENDERER_LINEBUCKET
#define LLMR_RENDERER_LINEBUCKET

#include "bucket.hpp"
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/vao.hpp>
#include <llmr/geometry/elements_buffer.hpp>
#include <memory>
#include <vector>

namespace llmr {

class Style;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LinejoinShader;
struct Coordinate;
struct pbf;

class LineBucket : public Bucket {
    typedef ElementGroup<LineShader> line_group_type;
    typedef ElementGroup<LinejoinShader> point_group_type;
public:
    LineBucket(const std::shared_ptr<LineVertexBuffer>& vertexBuffer,
               const std::shared_ptr<TriangleElementsBuffer>& triangleElementsBuffer,
               const std::shared_ptr<PointElementsBuffer>& pointElementsBuffer,
               const BucketDescription& bucket_desc);

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);

    bool empty() const;
    bool hasPoints() const;

    void drawLines(LineShader& shader);
    void drawPoints(LinejoinShader& shader);

public:
    const BucketGeometryDescription geometry;

private:

    std::shared_ptr<LineVertexBuffer> vertexBuffer;
    std::shared_ptr<TriangleElementsBuffer> triangleElementsBuffer;
    std::shared_ptr<PointElementsBuffer> pointElementsBuffer;

    const uint32_t vertex_start;
    const uint32_t triangle_elements_start;
    const uint32_t point_elements_start;

    std::vector<line_group_type> lineGroups;
    std::vector<point_group_type> pointGroups;
};

}

#endif
