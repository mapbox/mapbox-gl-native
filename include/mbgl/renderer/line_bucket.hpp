#ifndef MBGL_RENDERER_LINEBUCKET
#define MBGL_RENDERER_LINEBUCKET

#include "bucket.hpp"
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/style/style_bucket.hpp>

#include <memory>
#include <vector>

namespace mbgl {

class Style;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LinejoinShader;
struct pbf;

class LineBucket : public Bucket {
    typedef ElementGroup<1> triangle_group_type;
    typedef ElementGroup<1> point_group_type;
public:
    LineBucket(LineVertexBuffer& vertexBuffer,
               TriangleElementsBuffer& triangleElementsBuffer,
               PointElementsBuffer& pointElementsBuffer,
               const StyleBucketLine& properties);

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);

    bool hasPoints() const;

    void drawLines(LineShader& shader);
    void drawPoints(LinejoinShader& shader);

public:
    const StyleBucketLine &properties;

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
