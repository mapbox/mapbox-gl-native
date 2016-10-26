#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/element_group.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
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

    void drawCircles(CircleShader&, gl::Context&, PaintMode);

private:
    std::vector<CircleVertex> vertices;
    std::vector<gl::Triangle> triangles;

    std::vector<ElementGroup<CircleShader>> groups;

    optional<gl::VertexBuffer<CircleVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> indexBuffer;

    const MapMode mode;
};

} // namespace mbgl
