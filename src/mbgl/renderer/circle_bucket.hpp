#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/segment.hpp>
#include <mbgl/programs/circle_program.hpp>

namespace mbgl {

class CircleBucket : public Bucket {
public:
    CircleBucket(const MapMode);

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;

    bool hasData() const override;
    void addGeometry(const GeometryCollection&);

    gl::VertexVector<CircleVertex> vertices;
    gl::IndexVector<gl::Triangles> triangles;
    gl::SegmentVector<CircleAttributes> segments;

    optional<gl::VertexBuffer<CircleVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangles>> indexBuffer;

    const MapMode mode;
};

} // namespace mbgl
