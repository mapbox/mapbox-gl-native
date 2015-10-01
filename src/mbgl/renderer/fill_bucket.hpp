#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile.hpp>
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

    void upload(gl::GLObjectStore&) override;
    void render(Painter&, const StyleLayer&, const UnwrappedTileID&, const mat4&) override;
    bool hasData() const override;
    bool needsClipping() const override;

    void addGeometry(const GeometryCollection&);

    void drawElements(PlainShader&, gl::GLObjectStore&);
    void drawElements(PatternShader&, gl::GLObjectStore&);
    void drawVertices(OutlineShader&, gl::GLObjectStore&);
    void drawVertices(OutlinePatternShader&, gl::GLObjectStore&);

private:
    FillVertexBuffer vertexBuffer;
    TriangleElementsBuffer triangleElementsBuffer;
    LineElementsBuffer lineElementsBuffer;

    typedef ElementGroup<2> TriangleGroup;
    typedef ElementGroup<2> LineGroup;

    std::vector<std::unique_ptr<TriangleGroup>> triangleGroups;
    std::vector<std::unique_ptr<LineGroup>> lineGroups;
};

} // namespace mbgl
