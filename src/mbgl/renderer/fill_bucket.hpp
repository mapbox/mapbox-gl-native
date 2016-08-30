#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/geometry/elements_buffer.hpp>
#include <mbgl/geometry/fill_buffer.hpp>

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

    void upload(gl::ObjectStore&, gl::Config&) override;
    void render(Painter&, PaintParameters&, const style::Layer&, const RenderTile&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

    void drawElements(PlainShader&, gl::ObjectStore&, bool overdraw);
    void drawElements(PatternShader&, gl::ObjectStore&, bool overdraw);
    void drawVertices(OutlineShader&, gl::ObjectStore&, bool overdraw);
    void drawVertices(OutlinePatternShader&, gl::ObjectStore&, bool overdraw);

private:
    FillVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;

    typedef ElementGroup<4> TriangleGroup;
    typedef ElementGroup<4> LineGroup;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
    std::vector<std::unique_ptr<LineGroup>> lineGroups;
};

} // namespace mbgl
