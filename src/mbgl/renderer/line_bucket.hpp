#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/vao.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/line_buffer.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

#include <vector>

namespace mbgl {

class LineVertexBuffer;
class TriangleElementsBuffer;
class LineShader;
class LineSDFShader;
class LinepatternShader;

class LineBucket : public Bucket {
    using TriangleGroup = ElementGroup<6>;

public:
    LineBucket(uint32_t overscaling);
    ~LineBucket() override;

    void upload(gl::ObjectStore&, gl::Config&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);
    void addGeometry(const GeometryCoordinates& line);

    void drawLines(LineShader&, gl::ObjectStore&, bool overdraw);
    void drawLineSDF(LineSDFShader&, gl::ObjectStore&, bool overdraw);
    void drawLinePatterns(LinepatternShader&, gl::ObjectStore&, bool overdraw);

private:
    struct TriangleElement {
        TriangleElement(uint16_t a_, uint16_t b_, uint16_t c_) : a(a_), b(b_), c(c_) {}
        uint16_t a, b, c;
    };
    void addCurrentVertex(const GeometryCoordinate& currentVertex, double& distance,
            const Point<double>& normal, double endLeft, double endRight, bool round,
            GLint startVertex, std::vector<LineBucket::TriangleElement>& triangleStore);
    void addPieSliceVertex(const GeometryCoordinate& currentVertex, double distance,
            const Point<double>& extrude, bool lineTurnsLeft, GLint startVertex,
            std::vector<TriangleElement>& triangleStore);

public:
    style::LineLayoutProperties layout;

private:
    LineVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;

    GLint e1;
    GLint e2;
    GLint e3;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;

    const uint32_t overscaling;
};

} // namespace mbgl
