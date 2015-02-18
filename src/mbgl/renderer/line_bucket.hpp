#ifndef MBGL_RENDERER_LINEBUCKET
#define MBGL_RENDERER_LINEBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/util/vec.hpp>

#include <vector>

namespace mbgl {

class Style;
class StyleLayoutLine;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LinejoinShader;
class LineSDFShader;
class LinepatternShader;
struct pbf;

class LineBucket : public Bucket {
    typedef ElementGroup<3> triangle_group_type;
    typedef ElementGroup<1> point_group_type;

public:
    LineBucket(std::unique_ptr<const StyleLayoutLine> styleLayout,
               LineVertexBuffer &vertexBuffer,
               TriangleElementsBuffer &triangleElementsBuffer,
               PointElementsBuffer &pointElementsBuffer);
    ~LineBucket();

    virtual void render(Painter& painter, util::ptr<StyleLayer> layer_desc, const Tile::ID& id, const mat4 &matrix);
    virtual bool hasData() const;

    void addGeometry(pbf& data);
    void addGeometry(const std::vector<Coordinate>& line);

    bool hasPoints() const;

    void drawLines(LineShader& shader);
    void drawLineSDF(LineSDFShader& shader);
    void drawLinePatterns(LinepatternShader& shader);
    void drawPoints(LinejoinShader& shader);

public:
    const std::unique_ptr<const StyleLayoutLine> styleLayout;

private:
    LineVertexBuffer& vertexBuffer;
    TriangleElementsBuffer& triangleElementsBuffer;
    PointElementsBuffer& pointElementsBuffer;

    const size_t vertex_start;
    const size_t triangle_elements_start;
    const size_t point_elements_start;

    std::vector<std::unique_ptr<triangle_group_type>> triangleGroups;
    std::vector<std::unique_ptr<point_group_type>> pointGroups;
};

}

#endif
