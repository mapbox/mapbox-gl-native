#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

#include <vector>

namespace mbgl {

namespace style {
class BucketParameters;
} // namespace style

class LineBucket : public Bucket {
public:
    LineBucket(const style::BucketParameters&,
               const std::vector<const style::Layer*>&,
               const style::LineLayoutProperties&);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&) override;
    bool hasData() const override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;

    style::LineLayoutProperties::Evaluated layout;

    gl::VertexVector<LineLayoutVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    gl::SegmentVector<LineAttributes> segments;

    optional<gl::VertexBuffer<LineLayoutVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;

    std::unordered_map<std::string, LineProgram::PaintPropertyBinders> paintPropertyBinders;

private:
    void addGeometry(const GeometryCoordinates& line);

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

    std::ptrdiff_t e1;
    std::ptrdiff_t e2;
    std::ptrdiff_t e3;

    const uint32_t overscaling;
};

} // namespace mbgl
