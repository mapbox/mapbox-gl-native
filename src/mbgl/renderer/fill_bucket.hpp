#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/element_group.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/shader/plain_vertex.hpp>

#include <vector>
#include <memory>

namespace mbgl {

class OutlinePatternShader;
class PlainShader;
class PatternShader;
class OutlineShader;

class FillBucket : public Bucket {
public:
    FillBucket();
    ~FillBucket() override;

    void upload(gl::Context&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

    void drawElements(PlainShader&, gl::Context&, PaintMode);
    void drawElements(PatternShader&, gl::Context&, PaintMode);
    void drawVertices(OutlineShader&, gl::Context&, PaintMode);
    void drawVertices(OutlinePatternShader&, gl::Context&, PaintMode);

private:
    std::vector<PlainVertex> vertices;
    std::vector<gl::Line> lines;
    std::vector<gl::Triangle> triangles;

    std::vector<ElementGroup<OutlineShader, OutlinePatternShader>> lineGroups;
    std::vector<ElementGroup<PlainShader, PatternShader>> triangleGroups;

    optional<gl::VertexBuffer<PlainVertex>> vertexBuffer;
    optional<gl::IndexBuffer<gl::Line>> lineIndexBuffer;
    optional<gl::IndexBuffer<gl::Triangle>> triangleIndexBuffer;
};

} // namespace mbgl
