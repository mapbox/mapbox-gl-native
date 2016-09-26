#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/element_group.hpp>
#include <mbgl/shader/circle_vertex.hpp>

namespace mbgl {

class CircleShader;

class CircleBucket : public Bucket {
public:
    CircleBucket(const MapMode);
    ~CircleBucket() override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;

    bool hasData() const override;
    bool needsClipping() const override;
    void addGeometry(const GeometryCollection&);

    void drawCircles(CircleShader&, gl::Context&);

private:
    std::vector<CircleVertex> vertices;
    std::vector<gl::Triangle> triangles;

    using TriangleGroup = gl::ElementGroup<3>;
    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;

    optional<gl::VertexBuffer<CircleVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;

    const MapMode mode;
};

} // namespace mbgl
