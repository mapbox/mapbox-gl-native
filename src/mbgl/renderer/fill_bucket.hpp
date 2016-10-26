#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/element_group.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/shader/fill_vertex.hpp>

#include <vector>
#include <memory>

namespace mbgl {

class FillShader;
class FillPatternShader;
class FillOutlineShader;
class FillOutlinePatternShader;

class FillBucket : public Bucket {
public:
    FillBucket();
    ~FillBucket() override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

    void drawElements(FillShader&, gl::Context&, PaintMode);
    void drawElements(FillPatternShader&, gl::Context&, PaintMode);
    void drawVertices(FillOutlineShader&, gl::Context&, PaintMode);
    void drawVertices(FillOutlinePatternShader&, gl::Context&, PaintMode);

private:
    std::vector<FillVertex> vertices;
    std::vector<gl::Line> lines;
    std::vector<gl::Triangle> triangles;

    std::vector<ElementGroup<FillOutlineShader, FillOutlinePatternShader>> lineGroups;
    std::vector<ElementGroup<FillShader, FillPatternShader>> triangleGroups;

    optional<gl::VertexBuffer<FillVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Line>> lineIndexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> triangleIndexBuffer;
};

} // namespace mbgl
