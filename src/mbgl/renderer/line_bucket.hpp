#ifndef MBGL_RENDERER_LINEBUCKET
#define MBGL_RENDERER_LINEBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/geometry_tile.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/style_layout.hpp>
#include <mbgl/util/vec.hpp>

#include <vector>

namespace mbgl {

class Style;
class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LineSDFShader;
class LinepatternShader;

class LineBucket : public Bucket {
    using TriangleGroup = ElementGroup<3>;

public:
    LineBucket(LineVertexBuffer &vertexBuffer, TriangleElementsBuffer &triangleElementsBuffer);
    ~LineBucket() override;

    void upload() override;
    void render(Painter&, const StyleLayer&, const TileID&, const mat4&) override;
    bool hasData() const;

    void addGeometry(const GeometryCollection&);
    void addGeometry(const std::vector<Coordinate>& line);

    void drawLines(LineShader& shader);
    void drawLineSDF(LineSDFShader& shader);
    void drawLinePatterns(LinepatternShader& shader);

private:
    struct TriangleElement {
        TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
        uint16_t a, b, c;
    };
    void addCurrentVertex(const Coordinate& currentVertex, float flip, double distance,
            const vec2<double>& normal, float endLeft, float endRight, bool round,
            int32_t startVertex, std::vector<LineBucket::TriangleElement>& triangleStore);

public:
    StyleLayoutLine layout;

private:
    LineVertexBuffer& vertexBuffer;
    TriangleElementsBuffer& triangleElementsBuffer;

    const size_t vertex_start;
    const size_t triangle_elements_start;

    int32_t e1;
    int32_t e2;
    int32_t e3;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
};

}

#endif
