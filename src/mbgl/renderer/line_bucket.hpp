#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/element_group.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/shader/line_vertex.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

#include <vector>

namespace mbgl {

class LineShader;
class LineSDFShader;
class LinePatternShader;

class LineBucket : public Bucket {

public:
    LineBucket(uint32_t overscaling);
    ~LineBucket() override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);
    void addGeometry(const GeometryCoordinates& line);

    void drawLines(LineShader&, gl::Context&, PaintMode);
    void drawLineSDF(LineSDFShader&, gl::Context&, PaintMode);
    void drawLinePatterns(LinePatternShader&, gl::Context&, PaintMode);

private:
    struct TriangleElement {
        TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
        uint16_t a, b, c;
    };
    void addCurrentVertex(const GeometryCoordinate& currentVertex, double& distance,
            const Point<double>& normal, double endLeft, double endRight, bool round,
            std::size_t startVertex, std::vector<LineBucket::TriangleElement>& triangleStore);
    void addPieSliceVertex(const GeometryCoordinate& currentVertex, double distance,
            const Point<double>& extrude, bool lineTurnsLeft, std::size_t startVertex,
            std::vector<TriangleElement>& triangleStore);

public:
    style::LineLayoutProperties layout;

private:
    std::vector<LineVertex> vertices;
    std::vector<gl::Triangle> triangles;

    std::vector<ElementGroup<LineShader, LineSDFShader, LinePatternShader>> groups;

    optional<gl::VertexBuffer<LineVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;

    std::ptrdiff_t e1;
    std::ptrdiff_t e2;
    std::ptrdiff_t e3;

    const uint32_t overscaling;
};

} // namespace mbgl
