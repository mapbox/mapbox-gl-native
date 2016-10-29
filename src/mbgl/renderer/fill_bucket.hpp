#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/shader/fill_attributes.hpp>

#include <vector>

namespace mbgl {

class FillBucket : public Bucket {
public:
    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;

    void addGeometry(const GeometryCollection&);

    std::vector<FillVertex> vertices;
    std::vector<gl::Line> lines;
    std::vector<gl::Triangle> triangles;
    std::vector<gl::Segment> lineSegments;
    std::vector<gl::Segment> triangleSegments;

    optional<gl::VertexBuffer<FillVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Line>> lineIndexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> triangleIndexBuffer;
};

} // namespace mbgl
